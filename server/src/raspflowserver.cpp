// g++ -o server -I include raspflowserver.cpp lib/libserialport.dll.a lib/libwebsockets.dll.a
#define LIBSERIALPORT_DEBUG

#include <thread>
#include <algorithm>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <libserialport.h>
#include <libwebsockets.h>

#include <map>
#include <string>

// Global for serial
struct sp_port *port;
sp_return result;

// Message buffer only recognizing JSON data ending with \n
std::string messageBuffer;

// Nombre del puerto serie (ajusta según tu configuración)
const char *port_name = "COM5";  // Reemplaza con tu puerto serie


// Global for WS
#define sendLwsData(wsi, output, size) \
lws_write(wsi, (unsigned char*) output, size, LWS_WRITE_TEXT)

// The main connection
struct lws * mainWebsocketConnection;

// Buffer for assembling a complete WebSocket message
std::map<struct lws *, std::string> lwsInputBuffer;

static int interrupted;

void sigint_handler(int sig) {
	interrupted = 1;
}

static int callback_ws(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

// Información del servidor WebSocket
static struct lws_protocols protocols[] = {
	{ "ws", callback_ws, 0, 0 },
	{ NULL, NULL, 0, 0 }  // El último elemento debe ser nulo
};

// Configuración del servidor
static struct lws_context *context;

static int callback_ws(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED: {
			lwsl_warn("LWS_CALLBACK_ESTABLISHED\n");
			mainWebsocketConnection = wsi;
			break;
		}

		case LWS_CALLBACK_RECEIVE: {
			lwsl_warn("LWS_CALLBACK_RECEIVE\n");
			if( (unsigned int)lws_remaining_packet_payload(wsi) ) {//Si es que faltan datos por llegar
			//~ lwsl_user("Hay datos pendientes en la cola de carga útil.\n");
				std::map<struct lws *, std::string>::iterator messageBuffer = lwsInputBuffer.find(wsi);
				if( messageBuffer == lwsInputBuffer.end() ) {
					//~ lwsl_user("No hemos encontrado un amortiguador y se añadirá.\n");
					std::string message;
					message.append( (char *) in, len );
					lwsInputBuffer[wsi] = message;
				}
				else {
					//Añadimos mensaje al amortiguador existente.
					//~ lwsl_user("Añadimos mensaje al amortiguador existente.\n");
					messageBuffer->second.append((char *) in, len);
				}
				break;
			}
			else {
				//~ lwsl_user("NO hay datos pendientes en la cola de carga útil.\n");
				std::string * completeMessage = new std::string;

				std::map<struct lws *, std::string>::iterator messageBuffer = lwsInputBuffer.find(wsi);
				if( messageBuffer != lwsInputBuffer.end() ) {//Hay datos amortizados
					completeMessage->append(messageBuffer->second);
					completeMessage->append((char *) in, len);

					//Se debe borrar mensaje
					lwsl_err("Mensaje Pendiente no necesitado más.\n");
					lwsInputBuffer.erase( messageBuffer );
				}
				else {//No hubo amortización
				//~ lwsl_user("NO hubo datos amortizados.\n");
					completeMessage->resize(len);
					completeMessage->insert(0, (char *) in, len);
				}

				printf("Mensaje WS capturado: %s\n", completeMessage->c_str());
				completeMessage->append("!");

				// Send to serial port
				unsigned int timeout = 1000;
				result = sp_blocking_write(port, completeMessage->c_str(), completeMessage->length(), timeout);
				if(result != completeMessage->length()) {
					std::cout << "Result sending out serial: " << result << std::endl;
					std::cerr << "Error writing to serial port: " << sp_last_error_message() << std::endl;
				}
				else {
					std::cout << "\tMessage " << *completeMessage << " sent to serial port." << std::endl;
				}
			}
			break;
		}

		default:
			break;
	}

	return 0;
}

void runSerial() {
	// Abre el puerto serie
	result = sp_get_port_by_name(port_name, &port);
	if (result != SP_OK) {
		fprintf(stderr, "Error al obtener el puerto por nombre: %s\n", sp_last_error_message());
		//~ return 1;
	}

	// Read and write
	result = sp_open(port, SP_MODE_READ_WRITE );
	if (result != SP_OK) {
		fprintf(stderr, "Error al abrir el puerto serie: %s\n", sp_last_error_message());
		sp_free_port(port);
		//~ return 1;
	}

	sp_set_baudrate(port, 115200);
	sp_set_bits(port, 8);
	sp_set_parity(port, SP_PARITY_NONE);
	sp_set_stopbits(port, 1);

	//
	//~ char serialTemporalBuffer[2048];
	//~ result = sp_blocking_read(port, serialTemporalBuffer, sizeof(serialTemporalBuffer), 0);  // Lee datos del puerto serie

	sp_flush(port, SP_BUF_BOTH);

	printf("Con. establecida. Esperando mensajes...\n");

	// Buffer para acumular el contenido entre iteraciones
	std::string buffer;

	char serialBuffer[128];  // Tamaño del búfer para almacenar los datos recibidos

	// Bucle para leer mensajes
	while(1 && !interrupted) {
		result = sp_blocking_read(port, serialBuffer, sizeof(serialBuffer), 0);  // Lee datos del puerto serie

		//~ if(SP_OK != result) {
			//~ continue;
		//~ }

		if(result > 0) {
			printf("Mensaje Serial recibido: %.*s\n", result, serialBuffer);
			std::string receivedText(serialBuffer);

			// Combina el buffer anterior con el texto actual
			std::string combinedText = buffer + receivedText;

			// Inicializa la posición de inicio de la búsqueda
			size_t startPos = 0;

			// Bucle para buscar todas las instancias de "START" y "END"
			while(true) {
				// Busca la posición de "START" y "END" desde la última posición de inicio
				size_t currentStartPos = combinedText.find("{\"cmd\"", startPos);
				size_t endPos = combinedText.find("\n", currentStartPos);

				// Verifica si "{\"cmd" y "\n" están presentes y "{\"cmd" aparece antes de "\n"
				if(currentStartPos != std::string::npos && endPos != std::string::npos && currentStartPos < endPos) {
					// Extrae el contenido entre "{\"cmd" y "\n" (incluyendo "{\"cmd" y "\n")
					std::string contenido = combinedText.substr(currentStartPos, endPos - currentStartPos - 1);

					// Imprime el contenido
					printf("\n\nSerial contenido de longitud %d extraído: %s", contenido.length(), contenido.c_str());
					// Catch all and resend it.
					if(mainWebsocketConnection) {
						char buf[LWS_PRE + contenido.length()];
						std::copy(contenido.begin(), contenido.end(), buf + LWS_PRE);
						lws_write(mainWebsocketConnection, (unsigned char *) &buf[LWS_PRE], contenido.length(), LWS_WRITE_TEXT);
					}
					contenido.clear();

					// Actualiza la posición de inicio para la próxima búsqueda
					startPos = endPos + 2;
				}
				else {
					// Si no se encuentran más instancias, guarda el resto en el buffer para la siguiente iteración
					buffer = combinedText.substr(startPos);
					break;
				}
			}

		}
		else if(result < 0) {
			fprintf(stderr, "Error al leer desde el puerto serie: %s\n", sp_last_error_message());
			break;
		}
	}

	// Cierra el puerto serie
	sp_close(port);
	sp_free_port(port);
}

void runWebsocket() {
	const char *iface = NULL;
	int wsPort = 9000;  // Puerto del servidor WebSocket

	struct lws_context_creation_info info;

	memset(&info, 0, sizeof(info));
	info.port = wsPort;
	info.iface = iface;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	// Inicializa el contexto del servidor
	context = lws_create_context(&info);
	if (!context) {
		fprintf(stderr, "Error al crear el contexto del servidor WebSocket\n");
		//~ return -1;
	}

	printf("Servidor WebSocket iniciado en el puerto %d\n", wsPort);

	// Bucle principal del servidor WebSocket
	int n = 0;
	while(n >= 0 && !interrupted) {
		n = lws_service(context, 50);
	}

	// Libera recursos
	lws_context_destroy(context);
}

int main() {
	std::thread serialThread(runSerial);
	std::thread websocketThread(runWebsocket);

	serialThread.join();
	websocketThread.join();

	return 0;
}

// Callback function for receiving serial data
void receiveDataCallback(struct sp_port *port, void *user_data) {
	static std::string receivedData;  // Static variable to store received data
	char buffer[512];  // Buffer to read incoming data
	int bytes_read;

	// Read incoming data
	bytes_read = sp_nonblocking_read(port, buffer, sizeof(buffer));

	if (bytes_read > 0) {
		// Append the received data to the buffer
		receivedData.append(buffer, bytes_read);

		// Check if the received data contains the end delimiter
		size_t endPos = receivedData.find("\n");
		if (endPos != std::string::npos) {
			// Print the complete message
			std::cout << "Received message: " << receivedData.substr(0, endPos) << std::endl;

			// Clear the received data buffer
			receivedData.clear();
		}
	}
}
