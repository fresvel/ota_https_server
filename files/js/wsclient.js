const gateway = `wss://${window.location.host}/ws`;
let websocket;

window.addEventListener('load', onLoad);
        
    function initWebSocket() {
        console.log('Trying to open a WebSocket connection...');
        websocket = new WebSocket(gateway)
        websocket.onopen = onOpen
        websocket.onclose = onClose
        websocket.onmessage = onMessage// <-- add this line
    }

    function onOpen(event) {
        console.log('Connection opened');
    }
    function onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 2000);
    }

    function onMessage(event) {
        let state;
        //console.log("Mensaje recibido");

        if (event.data == "1") {
            state = "ON";
        }
        else {
            state = "OFF";
        }
        
        console.log(`${event.data}\n`);
    }
    
    function onLoad(event) {
        initWebSocket();
    }
    

    
    function toggle() {
        websocket.send('toggle mundo');
    }
