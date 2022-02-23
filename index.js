const firebaseConfig = {
    apiKey: "AIzaSyAizKNl8rPeJEqK67QuQeQ0SHdXtgwfjcM",
    authDomain: "toggle-button-8891e.firebaseapp.com",
    databaseURL: "https://toggle-button-8891e-default-rtdb.firebaseio.com",
    projectId: "toggle-button-8891e",
    storageBucket: "toggle-button-8891e.appspot.com",
    messagingSenderId: "1003737634780",
    appId: "1:1003737634780:web:f59ef75ee11660a8fea119",
    measurementId: "G-H9D53Z3CGX"
  };

  // Initialize Firebase
  firebase.initializeApp(firebaseConfig);
var curday = 0;

$(document).ready(function(){
  var database = firebase.database();
  const d = new Date();
  var day = d.getDay();

  var Led1Status;
  var Flow;
  var Usage;
  var flag = 0;	
	database.ref().on("value", function(snap){
		Led1Status = snap.val().Led1Status;
    Flow = snap.val().Flow;
    Usage = snap.val().Usage;
    var firebaseRef0 = firebase.database().ref().child("Flow");
    var firebaseRef1 = firebase.database().ref().child("Usage");

//     if(curday != day)
//     {
//       firebaseRef1.set(0);
//       curday = day;
//     }

		if(Led1Status == "1"){    // check from the firebase
			//$(".Light1Status").text("The light is off");
			document.getElementById("unact").style.display = "none";
			document.getElementById("act").style.display = "block";
			document.getElementById("Uvalu").innerHTML = Flow;
		} else {
			//$(".Light1Status").text("The light is on");
			document.getElementById("unact").style.display = "block";
			document.getElementById("act").style.display = "none";
			document.getElementById("Uvalu").innerHTML = 0;
		}
    // Update flow in HTML
    document.getElementById("fvalu").innerHTML = Usage;
	});

    var firebaseRef = firebase.database().ref().child("Led1Status");




    function myTimer(){
	    
      if( Flow < 0.5 && Led1Status == '1' && flag)
      {
         firebaseRef.set("0");
	 alert('Water level low!!! fill water');
	 flag = 0;     
      }
      else if(Flow < 0.5 && Led1Status == '1' && !flag)
      {
	flag = 1;
      }	      
    }
    setInterval(myTimer, 5000);

    $(".toggle-btn").click(function(){

		if(Led1Status == "1"){    // post to firebase
			firebaseRef.set("0");
			Led1Status = "0";
		} else {
			firebaseRef.set("1");
			Led1Status = "1";
		}
	})
});
