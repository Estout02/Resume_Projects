
var h = document.createElement("h1");
             
var t = document.createTextNode("Welcome to the Chat server");
t.id = "h1";      
h.appendChild(t); 
document.body.appendChild(t);

lineBreak = document.createElement("br");
document.body.appendChild(lineBreak);
lineBreak = document.createElement("br");
document.body.appendChild(lineBreak);
lineBreak = document.createElement("br");
document.body.appendChild(lineBreak);



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
document.body.appendChild(pickRoom);

linebreak = document.createElement("br");
document.body.appendChild(linebreak);   

//connect to chat room
let connect = document.createElement("button");
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
    

    uName = document.createTextNode(userName);
    cRoom = document.createTextNode(chatRoom);

    
    infoBox.appendChild(cRoom);

    document.body.appendChild(infoBox);



    let textChatBox = document.createElement("DIV");
    textChatBox.id = "Chatbox";
    document.body.appendChild(textChatBox);

    let chatText = document.createElement("INPUT");
    chatText.id = "Textbox";
    textChatBox.appendChild(chatText);
    

    let chatbtn = document.createElement("button");
    chatbtn.innerHTML = "SEND";
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
            newName = document.createTextNode(messageParsed.user);
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
}