const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector("#authentication-bar");

// Elements for sensor readings
const tempElement = document.getElementById("temp");
const humElement = document.getElementById("hum");
const souElement = document.getElementById("sou");
const movElement = document.getElementById("mov");

// MANAGE LOGIN/LOGOUT UI
const setupUI = (user) => {
  if (user) {
    //toggle UI elements
    loginElement.style.display = 'none';
    contentElement.style.display = 'block';
    authBarElement.style.display ='block';
    userDetailsElement.style.display ='block';
    userDetailsElement.innerHTML = user.email;

    // get user UID to get data from database
    var uid = user.uid;
    console.log(uid);

    // Database paths
    var dbPathTemp = 'ESP032/Dados_Coletados/Temperatura';
    var dbPathHum = 'ESP032/Dados_Coletados/Humidade';
    var dbPathSou = 'ESP032/Dados_Coletados/Som';
    var dbPathMov = 'ESP032/Dados_Coletados/Movimento';

    // Database references
    var dbRefTemp = firebase.database().ref().child(dbPathTemp);
    var dbRefHum = firebase.database().ref().child(dbPathHum);
    var dbRefSou = firebase.database().ref().child(dbPathSou);
    var dbRefMov = firebase.database().ref().child(dbPathMov);


    // Update page with new readings
    dbRefTemp.on('value', snap => {
      tempElement.innerText = snap.val().toFixed(2);
    });

    dbRefHum.on('value', snap => {
      humElement.innerText = snap.val().toFixed(2);
    });

    dbRefSou.on('value', snap => {
      souElement.innerText = snap.val();
    });

    dbRefMov.on('value', snap => {
     movElement.innerText = snap.val();
    });

  // if user is logged out
  } else{
    // toggle UI elements
    loginElement.style.display = 'block';
    authBarElement.style.display ='none';
    userDetailsElement.style.display ='none';
    contentElement.style.display = 'none';
  }
}