import firebase from "firebase/app";
import "firebase/database"

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
firebase.initializeApp(firebaseConfig);

//initialize realtime databse and get reference
const database = firebase.database();

export { database };
