window.onload = function(){

var socket = new WebSocket("ws://" + location.host);

socket.onopen = function(){
    socket.onmessage = function(event){
        console.log(event.data);
    }
    console.log("opened");
    socket.send("hello");
    console.log(socket.readyState);
}

}

