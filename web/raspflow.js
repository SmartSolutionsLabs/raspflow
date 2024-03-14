const testLocal = false

const inputs = new Array(5);

const blockPattern = {
	"delay": {
		"type": -2,
		"in": 1,
		"out": 1
	},
	"start": {
		"type": 1,
		"in": 0,
		"out": 1,
		"withId": false
	},
	"end": {
		"type": 0,
		"in": 1,
		"out": 0,
		"withId": false
	},
	"relay": {
		"type": 2,
		"in": 1,
		"out": 1,
		"withId": true,
		"withValue": true,
		"withDelay": true
	},
	"motor": {
		"type": 3,
		"in": 1,
		"out": 1,
		"withId": true,
		"withValue": true,
		"withDelay": true
	},
	"sensor": {
		"type": 4,
		"in": 1,
		"out": 1,
		"withId": true,
		"withValue": true,
		"withDelay": true
	},
	"conditional": {
		"type": -1,
		"in": 1,
		"out": 2
	}
}

const editor = new Drawflow(drawflow)
editor.reroute = true
editor.reroute_fix_curvature = true
editor.force_first_input = false

/*
editor.createCurvature = function(start_pos_x, start_pos_y, end_pos_x, end_pos_y, curvature_value, type) {
	var center_x = ((end_pos_x - start_pos_x)/2)+start_pos_x
	return ' M ' + start_pos_x + ' ' + start_pos_y + ' L '+ center_x +' ' +  start_pos_y  + ' L ' + center_x + ' ' +  end_pos_y  + ' L ' + end_pos_x + ' ' + end_pos_y
}*/

let dataToImport = []
fetch('./data-simple.json').then(response => {
	dataToImport = response.json()
	dataToImport.then(function(e) {
		editor.import(e)
		console.log("imported")
	})
})
editor.start()

// Events!
//~ editor.on('nodeCreated', function(drawflow) {
	//~ console.log("Node created " + drawflow)
//~ })

//~ editor.on('nodeRemoved', function(drawflow) {
	//~ console.log("Node removed " + drawflow)
//~ })

//~ editor.on('nodeSelected', function(drawflow) {
	//~ console.log("Node selected " + drawflow)
//~ })

//~ editor.on('moduleCreated', function(name) {
	//~ console.log("Module Created " + name)
//~ })

//~ editor.on('moduleChanged', function(name) {
	//~ console.log("Module Changed " + name)
//~ })

//~ editor.on('connectionStart', function(connection) {
	//~ console.log('Connection start')
	//~ console.log(connection)
//~ })

//~ // Warn if start with end
//~ editor.on('connectionCreated', function(connection) {
	//~ console.log(connection)
//~ })

//~ editor.on('connectionRemoved', function(connection) {
	//~ console.log('Connection removed')
	//~ console.log(connection)
//~ })

//~ editor.on('mouseMove', function(position) {
	//~ console.log('Position mouse x:' + position.x + ' y:'+ position.y)
//~ })

//~ editor.on('nodeMoved', function(drawflow) {
	//~ console.log("Node moved " + drawflow)
//~ })

//~ editor.on('zoom', function(zoom) {
	//~ console.log('Zoom level ' + zoom)
//~ })

//~ editor.on('addReroute', function(drawflow) {
	//~ console.log("Reroute added " + drawflow)
//~ })

//~ editor.on('removeReroute', function(drawflow) {
	//~ console.log("Reroute removed " + drawflow)
//~ })
/* DRAG EVENT */

function traverseChildNodes(node) {
	console.log(`A ${node.name}.`)
	let output
	for(let outputLength = blockPattern[node.name].out; outputLength > 0; --outputLength) {
		let i = -1
		let someOutput
		while(someOutput = node.outputs[`output_${outputLength}`]) {
			// If not exists connections or if pins are unconnected
			if(!someOutput.connections || someOutput.connections.length == 0) {
				break
			}
			output = someOutput.connections[++i]
			if(!output) {
				break
			}
			traverseChildNodes( editor.getNodeFromId(output.node) )
		}
	}
}

function sendDiagram() {
	const drawflowModule = editor.module
	const nodes = editor.drawflow.drawflow[drawflowModule].data
	let startNode
	for(const key in nodes) {
		if(editor.drawflow.drawflow[drawflowModule].data[key].name === "start") {
			// Start found
			startNode = editor.drawflow.drawflow[drawflowModule].data[key]
		}
	}

	if(!startNode) {
		Swal.fire("There's no start.")
		return
	}

	// Traverse all nodes starting from start
	traverseChildNodes(startNode)
}

/* Mouse and Touch Actions */

var elements = document.getElementsByClassName('drag-drawflow')
for(let i = 0; i < elements.length; i++) {
	elements[i].addEventListener('touchend', drop, false)
	elements[i].addEventListener('touchmove', positionMobile, false)
	elements[i].addEventListener('touchstart', drag, false )
}

var mobile_item_selec = ''
var mobile_last_move = null

 function positionMobile(ev) {
	mobile_last_move = ev
}

 function allowDrop(ev) {
	ev.preventDefault()
}

function drag(ev) {
	if(ev.type === "touchstart") {
		mobile_item_selec = ev.target.closest(".drag-drawflow").getAttribute('data-node')
	}
	else {
		ev.dataTransfer.setData("node", ev.target.getAttribute('data-node'))
	}
}

function drop(ev) {
	if (ev.type === "touchend") {
		var parentdrawflow = document.elementFromPoint( mobile_last_move.touches[0].clientX, mobile_last_move.touches[0].clientY).closest("#drawflow")
		if(parentdrawflow != null) {
			addNodeToDrawFlow(mobile_item_selec, mobile_last_move.touches[0].clientX, mobile_last_move.touches[0].clientY)
		}
		mobile_item_selec = ''
	}
	else {
		ev.preventDefault()
		var data = ev.dataTransfer.getData("node")
		addNodeToDrawFlow(data, ev.clientX, ev.clientY)
	}
}

function addNodeToDrawFlow(name, pos_x, pos_y) {
	if(editor.editor_mode === 'fixed') {
		return false
	}

	// Only one start but many ends
	if(name == "start"/* || name == "end"*/) {
		const drawflowModule = editor.module
		const nodes = editor.drawflow.drawflow[drawflowModule].data
		for(const key in nodes) {
			if(editor.drawflow.drawflow[drawflowModule].data[key].name === name) {
				Swal.fire(`${name} already exists.`)
				return false
			}
		}
	}

	pos_x = pos_x * ( editor.precanvas.clientWidth / (editor.precanvas.clientWidth * editor.zoom)) - (editor.precanvas.getBoundingClientRect().x * ( editor.precanvas.clientWidth / (editor.precanvas.clientWidth * editor.zoom)))
	pos_y = pos_y * ( editor.precanvas.clientHeight / (editor.precanvas.clientHeight * editor.zoom)) - (editor.precanvas.getBoundingClientRect().y * ( editor.precanvas.clientHeight / (editor.precanvas.clientHeight * editor.zoom)))


	switch(name) {
		case 'start':
			var start = `
			<div>
				<div class="title-box"><i class="fab fa-start"></i> Start</div>
			</div>
			`
			editor.addNode('start', 0, 1, pos_x, pos_y, 'start', {}, start )
			break
		case 'end':
			var end = `
			<div>
				<div class="title-box"><i class="fab fa-target"></i> End</div>
			</div>
			`
			editor.addNode('end', 1, 0, pos_x, pos_y, 'end', {}, end )
			break
		case 'relay':
			var relay = `
			<div>
				<div class="title-box"><i class="fab fa-telegram-plane"></i> Relay</div>
				<div class="box">
					<p>Select relay No.</p>
					<select df-channel>
						<option hidden selected>Choose...</option>
						<option value="1">1</option>
						<option value="2">2</option>
						<option value="3">3</option>
						<option value="4">4</option>
					</select>
				</div>
				<div class="box">
					<label>State:</label>
					<select df-powered>
						<option hidden selected>Choose...</option>
						<option value="1">ON</option>
						<option value="0">OFF</option>
					</select>
				</div>
				<div class="box">
					<label>With previous delay:</label>
					<input type="number" df-delay placeholder="Delay value">
				</div>
			</div>
			`
			editor.addNode('relay', 1, 1, pos_x, pos_y, 'relay', {}, relay )
			break
		case 'motor':
			var motor = `
			<div>
				<div class="title-box"><i class="fab fa-telegram-plane"></i> Motor</div>
				<div class="box">
					<p>Select Motor No.</p>
					<select df-channel>
						<option hidden selected>Choose...</option>
						<option value="1">1</option>
						<option value="2">2</option>
						<option value="3">3</option>
						<option value="4">4</option>
					</select>
				</div>
				<div class="box">
					<label>State:</label>
					<select df-powered>
						<option hidden selected>Choose...</option>
						<option value="1">ON</option>
						<option value="0">OFF</option>
					</select>
				</div>
				<div class="box">
					<label>With previous delay:</label>
					<input type="number" df-delay placeholder="Delay value">
				</div>
			</div>
			`
			editor.addNode('motor', 1, 1, pos_x, pos_y, 'motor', {}, motor )
			break
		case 'delay':
			var delay = `
			<div>
				<div class="title-box"><i class="fab fa-telegram-plane"></i> Delay</div>
				<div class="box">
					<p>Time:</p>
					<input type="number" df-time>
				</div>
			</div>
			`
			editor.addNode('delay', 1, 1, pos_x, pos_y, 'delay', {}, delay )
			break
		case 'sensor':
			var sensor = `
			<div>
				<div class="title-box"><i class="fab fa-telegram-plane"></i> Sensor</div>
				<div class="box">
					<p>Select sensor No.</p>
					<select df-channel>
						<option hidden selected>Choose...</option>
						<option value="1">1</option>
						<option value="2">2</option>
						<option value="3">3</option>
						<option value="4">4</option>
					</select>
				</div>
				<div class="box">
					<p>Value:</p>
					<input type="text" df-name readonly placeholder="Read-only">
				</div>
				<div class="box">
					<label>With previous delay:</label>
					<input type="number" df-delay placeholder="Delay value">
				</div>
			</div>
			`
			editor.addNode('sensor', 1, 1, pos_x, pos_y, 'sensor', {}, sensor )
			break
		case 'conditional':
			var conditional = `
			<div>
				<div class="title-box"><i class="fab fa-telegram-plane"></i> Conditional</div>
				<div class="box">
					<p>If data of:</p>
					<select df-valueLeft>
						<option hidden selected>Choose...</option>
						<option value="1">Sensor 1</option>
						<option value="2">Sensor 2</option>
						<option value="3">Sensor 3</option>
						<option value="4">Sensor 4</option>
					</select>
					<select df-condition>
						<option hidden selected>Choose...</option>
						<option value="g">&gt</option>
						<option value="l">&lt</option>
						<option value="ge">&gt=</option>
						<option value="le">&gt=</option>
						<option value="ne">!=</option>
					</select>
					<input type="text" df-valueRight>
				</div>
			</div>
			`
			editor.addNode('conditional', 1, 2, pos_x, pos_y, 'conditional', {}, conditional )
			break
		default:
	}
}

var transform = ''
function showpopup(e) {
	e.target.closest(".drawflow-node").style.zIndex = "9999"
	e.target.children[0].style.display = "block"
	//document.getElementById("modalfix").style.display = "block"

	//e.target.children[0].style.transform = 'translate('+translate.x+'px, '+translate.y+'px)'
	transform = editor.precanvas.style.transform
	editor.precanvas.style.transform = ''
	editor.precanvas.style.left = editor.canvas_x +'px'
	editor.precanvas.style.top = editor.canvas_y +'px'
	console.log(transform)

	//e.target.children[0].style.top  =  -editor.canvas_y - editor.container.offsetTop +'px'
	//e.target.children[0].style.left  =  -editor.canvas_x  - editor.container.offsetLeft +'px'
	editor.editor_mode = "fixed"

}

 function closemodal(e) {
	 e.target.closest(".drawflow-node").style.zIndex = "2"
	 e.target.parentElement.parentElement.style.display  ="none"
	 //document.getElementById("modalfix").style.display = "none"
	 editor.precanvas.style.transform = transform
	 editor.precanvas.style.left = '0px'
	 editor.precanvas.style.top = '0px'
	editor.editor_mode = "edit"
 }

function changeModule(event) {
	var all = document.querySelectorAll(".menu ul li")
	for(var i = 0; i < all.length; i++) {
		all[i].classList.remove('selected')
	}
	event.target.classList.add('selected')
}

function changeMode(option) {
	//console.log(lock.id)
	if(option == 'lock') {
		lock.style.display = 'none'
		unlock.style.display = 'block'
	} else {
		lock.style.display = 'block'
		unlock.style.display = 'none'
	}
}

class Raspflow {
	#ws
	#uselessString = "Mig*"
	#playing = false

	get playing() {
		return this.#playing
	}

	constructor() {
		//~ this.ws = new WebSocket()
		//~ this.ws.on
		console.log("Raspflow")

		if(!testLocal) {
			this.#connect()
		}
	}

	stop() {
		this.#playing = false
		editor.editor_mode = "edit"
		changeMode("unlock")

		play.style.display = "block"
		stopper.style.display = "none"
	}

	// Only starts outputs of Start, nothing else
	start() {
		this.#playing = true
		if(!testLocal) {
			// When is connected
			if(!this.#ws || this.#ws.readyState !== WebSocket.OPEN) {
				Swal.fire("Sin conexión a central.")
				return
			}
		}

		editor.editor_mode = "fixed"
		changeMode("lock")

		play.style.display = "none"
		stopper.style.display = "block"
		const drawflowModule = editor.module
		const nodes = editor.drawflow.drawflow[drawflowModule].data
		let startNode
		for(const key in nodes) {
			if(editor.drawflow.drawflow[drawflowModule].data[key].name === "start") {
				// Start found
				startNode = editor.drawflow.drawflow[drawflowModule].data[key]

				//~ debugger

				let payload = {
					cmd: 1,
					data: {
						type: 1,
						uuid: startNode.id,
					}
				}

				if(!testLocal) {
					this.#ws.send(JSON.stringify(payload))
				}
				else {
					this.#traverseChildNodes(startNode)
				}

				// That's all
				break
			}
		}

		if(!startNode) {
			Swal.fire("There's no start.")
			return
		}
	}

	// It makes and sends sibling steps
	#traverseChildNodes(node) {
		console.log(`A ${node.name}.`)

		// If we found a end means that we must stop everything
		if(node.name == "end") {
			this.stop()
			Swal.fire("Se alcanzó el fin.")
			return
		}

		let withConnectionPointer = 0
		// Conditional only
		if(node.name == "conditional") {
			withConnectionPointer = 1
		}

		let output
		let payload
		for(let outputLength = blockPattern[node.name].out; outputLength > 0; --outputLength) {

			// Go to connection pointer
			// No need to know if true or false, just follow path
			if(withConnectionPointer != 0 && outputLength != withConnectionPointer) {
				continue
			}

			let i = -1
			let someOutput
			while(someOutput = node.outputs[`output_${outputLength}`]) {
				// If not exists connections or if connections are unconnected
				if(!someOutput.connections || someOutput.connections.length == 0) {
					break
				}
				output = someOutput.connections[++i]
				if(!output) {
					break
				}

				output = editor.getNodeFromId(output.node)

				payload = {
					cmd: 1,
					data: {
						type: blockPattern[output.name].type,
						uuid: output.id,
					}
				}

				if(blockPattern[output.name].withId) {
					payload.data.id = parseInt(output.data.channel)
				}
				if(blockPattern[output.name].withValue) {
					payload.data.set = parseInt(output.data.powered)
				}
				if(blockPattern[output.name].withDelay) {
					payload.data.delay = parseInt(output.data.delay)
					if(isNaN(payload.data.delay)) {
						payload.data.delay = 0
					}
				}

				if(!testLocal) {
					if(withConnectionPointer == 0) {
						this.#ws.send(JSON.stringify(payload))
					}
					else {
						//~ debugger
						this.#traverseChildNodes(output)
					}
				}
				else {
					this.#traverseChildNodes(output)
				}
			}

			// Here we know that we must execute only one path
			if(withConnectionPointer != 0) {
				console.log("Conditional pointer was executed")
				break
			}
		}
	}

	// To WebSocket
	#connect() {
		//~ this.#ws = new ReconnectingWebSocket("ws://192.168.18.33:8765")
		this.#ws = new ReconnectingWebSocket("ws://127.0.0.1:9000")
		this.#ws.onmessage = this.handleRemoteAnswer.bind(this)
		this.#ws.onopen = this.#handleOpenConnection.bind(this)
		this.#ws.onerror = this.#handleRemoteError.bind(this)
		this.#ws.onclose = this.#handleRemoteClose.bind(this)
	}

	#handleOpenConnection() {
		header.classList.add('connected')
		console.log(`Raspflow socket opened to ${this.#uselessString}.`)
	}

	// Parse answer
	handleRemoteAnswer(answer) {
		let data
		try {
			console.log(answer.data)
			data = JSON.parse(answer.data)
		}
		catch(e) {
			return
		}

		// Here we parse answer for stepping
		switch(data.cmd) {
			case "status":
				//~ debugger
				if(this.#playing) {
					if(data.data.uuid) {
						if(data.data.type == "Sensor") {
							inputs[data.data.id] = data.data.value
						}
						this.#traverseChildNodes( editor.getNodeFromId(data.data.uuid) )
					}
					else {
						Swal.fire("Error de UUID remoto")
					}
				}
				break
			case "hold":
				console.info("hold")
				break
			default:
				//~ debugger
		}
	}

	#handleRemoteClose() {
		header.classList.remove('connected')
		console.log(`ws: ${this.#uselessString} was closed.`)
		this.stop()
	}

	#handleRemoteError() {
		console.log(`ws: Error with ${this.#uselessString}.`)
	}

	// Send message
	controlDevice(message) {
		this.#ws.send(message)
	}
}

var raspflow = new Raspflow()
