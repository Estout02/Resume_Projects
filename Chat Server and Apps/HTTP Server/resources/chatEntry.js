
var h = document.createElement("h1");
             
var t = document.createTextNode("Welcome to the Chat server");
t.id = "h1";      
h.appendChild(t); 
document.body.appendChild(t);





// Insert Username
var userText = document.createElement("HEADER");
userText.innerHTML = "Username";
document.body.appendChild(userText);

let userNameInput = document.createElement("input");
document.body.appendChild(userNameInput);


lineBreak = document.createElement("br");
document.body.appendChild(lineBreak);


//Insert chat room name
let roomName = document.createElement("HEADER");
roomName.innerHTML = "Roomname";
document.body.appendChild(roomName);

let pickRoom = document.createElement("input");
pickRoom.id = "roomInput";
document.body.appendChild(pickRoom);

linebreak = document.createElement("br");
document.body.appendChild(linebreak);   

//connect to chat room
let connect = document.createElement("button");
connect.id = "connectButton";
connect.innerHTML = "Connect to Chat";
document.body.appendChild(connect);



//create connection

let wsurl = "ws://" + location.host;
    let ws = new WebSocket(wsurl);


connect.onclick = function(){

  //clear screen  

    let userName = userNameInput.value;
    let chatRoom = pickRoom.value;
    
    
    document.body.removeChild(userText);
    document.body.removeChild(userNameInput);
    document.body.removeChild(roomName);
    document.body.removeChild(lineBreak);
    document.body.removeChild(pickRoom);
    document.body.removeChild(connect);

// create chatbox
    let infoBox = document.createElement("DIV");
    infoBox.id = "InfoBox";
    

    currentRoom = document.createTextNode("Current Room:");
    cRoom = document.createTextNode(chatRoom);


    infoBox.appendChild(currentRoom);
    infoBox.appendChild(cRoom);

    document.body.appendChild(infoBox);



    let textChatBox = document.createElement("DIV");
    textChatBox.id = "Chatbox";
    document.body.appendChild(textChatBox);

    let chatText = document.createElement("INPUT");
    chatText.id = "Textbox";
    textChatBox.appendChild(chatText);
    

    let chatbtn = document.createElement("button");
    chatbtn.id = 'chatbtn';
    chatbtn.innerHTML = "SEND MESSAGE";
    textChatBox.appendChild(chatbtn);
    let messageDiv = document.createElement("DIV");
    let userDiv = document.createElement("DIV");
    let messageB = document.createElement("DIV");
    messageB.id = "Mboard";
    document.body.appendChild(messageB);


   if(ws.readyState){

        //send request
        ws.send("join " + chatRoom);

        //add completeMessage to doc

        chatbtn.onclick = function(){

            let message = chatText.value;
            let socSend = userName + " " + message;

            ws.send(socSend);

            chatText.value = "";

        }



        ws.onmessage = function (event) {

            //parse JSON to get event data
            let messageParsed  = JSON.parse(event.data);
            let newMessage = document.createElement("DIV");
            newMessage.id = "newMessage";
            let newName = document.createTextNode(messageParsed.user);
            newName.id = "newestName";
            words = document.createTextNode(messageParsed.message);

            //use to see own messages differently then others for clarity
            if(messageParsed.user == userName){
                newMessage.classList.add("myMessage");
            }else{
                newMessage.classList.add("notMyMessage");
            }

            //create message and add to message board
            messageDiv.innerHTML = newMessage;
            let space = document.createElement("br");
            newMessage.appendChild(newName);
            newMessage.appendChild(space);
            newMessage.appendChild(words);
            messageB.appendChild(newMessage);

        }



    }

    var input = document.getElementById("Textbox");

    // Execute a function when the user releases a key on the keyboard
    input.addEventListener("keyup", function(event) {
        // Number 13 is the "Enter" key on the keyboard
        if (event.keyCode === 13) {
            // Cancel the default action, if needed
            event.preventDefault();
            // Trigger the button element with a click
            document.getElementById("chatbtn").click();
        }
    });

    var inputNext = document.getElementById("roomInput");

    // Execute a function when the user releases a key on the keyboard
    inputNext.addEventListener("keyup", function(event) {
        // Number 13 is the "Enter" key on the keyboard
        if (event.keyCode === 13) {
            // Cancel the default action, if needed
            event.preventDefault();
            // Trigger the button element with a click
            document.getElementById("connectButton").click();
        }
    });
}