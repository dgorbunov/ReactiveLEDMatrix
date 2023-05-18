import { useState, useEffect } from "react";
import { initializeApp } from "firebase/app";
import { getDatabase, ref, onValue, off, set } from "firebase/database";
import './App.css'

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyDlEYnZe4Smckppvzi4M2JI5DNgkeLAsXc",
  authDomain: "led-demo-909df.firebaseapp.com",
  databaseURL: "https://led-demo-909df-default-rtdb.firebaseio.com",
  projectId: "led-demo-909df",
  storageBucket: "led-demo-909df.appspot.com",
  messagingSenderId: "679497006360",
  appId: "1:679497006360:web:38024117f790cb8faa326d"
};


// Initialize Firebase
const firebaseApp = initializeApp(firebaseConfig);

//initialize realtime databse and get reference
const database = getDatabase(firebaseApp);


function App() {
  const [ledMode, setLedMode] = useState("");

  var modes = {
    0: "Toggle",
    1: "distanceColor",
    2: "distanceBrightness",
    3: "distanceColorBrightness",
    4: "heatMap",
    5: "paint",
    6: "paintNeg",
  };

  useEffect(() => {
    const ledModeRef = ref(database, "Led1Mode");
  
    // Listen to changes in the database
    const onLedModeChange = (snapshot) => {
      const data = snapshot.val();
      if (modes.hasOwnProperty(data)) {
        const modeName = modes[data];
        setLedMode(modeName);
        console.log("led mode: " + modeName);
      } else {
        console.warn("Invalid mode value: " + data);
      }
    };
    
  
    onValue(ledModeRef, onLedModeChange);
  
    // Cleanup function
    return () => {
      off(ledModeRef, onLedModeChange);
    };
  }, []);
  

  // handle  button clicks
  const handleLedStatusChange = (newMode) => {
    const ledModeRef = ref(database, "Led1Mode");
    set(ledModeRef, newMode);
    const modeName = modes[newMode];
    setLedMode(modeName);
    console.log("led mode: " + modeName);
  };
  


  return (
    <div>
      <div className="title">
        <h1 >LED Control Panel</h1>
      </div>
      
      {/* buttons */}
      <div className="buttons">
        <button onClick={() => handleLedStatusChange(0)} className="toggle-btn" value="Toggle">Paint Brightness</button>
        <button onClick={() => handleLedStatusChange(1)} className="distanceColor-btn" value="distanceColor">Gradient Color</button>
        <button onClick={() => handleLedStatusChange(2)} className="distanceBrightness-btn" value="distanceBrightness">Gradient Brightness</button>
        <button onClick={() => handleLedStatusChange(3)} className="distanceColorBrightness-btn" value="distanceColorBrightness">Rainbow Gradient</button>
        <button onClick={() => handleLedStatusChange(4)} className="heatmap-btn" value="heatMap">Thermal Map Mode</button>
        <button onClick={() => handleLedStatusChange(6)} className="paint-btn" value="paint">Paint Mode</button>
        <button onClick={() => handleLedStatusChange(7)} className="paintNeg-btn" value="paintNeg">Paint Negative Mode</button>
      </div>

    </div>
  );
}

export default App;
