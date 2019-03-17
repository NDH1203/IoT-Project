document.onload = function (e) {

}

var main = document.getElementById('main');

var stateRefs = new Array();
var stateElement = new Array();
var bStates = new Array();
var states = new Array();
var devicesCountRef = firebase.database().ref('User1/devicesCount');
var dCount = 0;
devicesCountRef.on('value', function (snapshot) {
    dCount = snapshot.val();
    console.log(dCount);
    UpdateDevicesList();
    DevicesStateUpdate();
    AddClickListener();
});

function UpdateDevicesList() {
    for (let i = 0; i < dCount; i++) {
        var t = i + 1;
        main.innerHTML += '<div class="devices"><span>Device ' + t + ': </span><span id="state' + t + '"></span><button id="bState' + t + '">On</button></div>';
    }
}

function DevicesStateUpdate() {
    for (let i = 0; i < dCount; i++) {
        var t = i + 1;
        stateElement[i] = document.getElementById('state' + t);
        bStates[i] = document.getElementById('bState'+t);
        stateRefs[i] = firebase.database().ref('User1/devices/device' + t);
        stateRefs[i].on('value', function (snapshot) {
            stateElement[i].innerText = snapshot.val();
            if(snapshot.val()){
                bStates[i].innerText = 'On';
            }
            else{
                bStates[i].innerText = 'Off';
            }
            states[i] = snapshot.val();
        });
    }
}
function AddClickListener(){
    for (let i = 0; i < dCount; i++) {
        bStates[i].addEventListener('click', function(){
            var tmp = i+1;
            var t = {};
            t['device'+tmp] = !states[i];
            console.log('fired');
            console.log(t);
            firebase.database().ref('User1/devices/').update(t);
        });
    }
}
function ButtonSwitch(buttonID){
    var t = {};
    t['device'+buttonID] = !states[buttonID-1];
    console.log('fired');
    console.log(t);
    firebase.database().ref('User1/devices/').update(t);
}