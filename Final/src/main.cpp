#include <WiFi.h>       // Library for connecting to Wi-Fi networks
#include <Arduino.h>    // Core library for Arduino boards
#include <Wire.h>       // Library for I2C communication
#include <WebServer.h>  // Library for creating a web server
#include <HTTPClient.h> // Library for making HTTP requests
#include <time.h>       // Library for time-related functions

const char *ssid = "";           // Wi-Fi network SSID
const char *password = "";    // Wi-Fi network password
const int dotSensor1Pin = 12;           // Pin number for dot sensor 1
const int dotSensor2Pin = 13;           // Pin number for dot sensor 2
const int analogPin = A0;               // Analog pin number
const char *ntpServer = "pool.ntp.org"; // NTP server to synchronize time
const long gmtOffset_sec = 5 * 3600 + 30 * 60; // Adjust for your time zone offset in seconds
const int daylightOffset_sec = 0;              // Adjust for daylight saving time offset in seconds
time_t now;                    // Variable to store the current time (UNIX timestamp)
struct tm timeinfo;            // Structure to store time information (e.g., hours, minutes, seconds, etc.)
char timeString[20];           // Character array to store the formatted time string (up to 20 characters)

const IPAddress staticIP(192, 168, 1, 100); // Replace with your desired IP address
const IPAddress gateway(192, 168, 1, 1);    // Replace with your gateway IP address
const IPAddress subnet(255, 255, 255, 0);   // Replace with your subnet mask
IPAddress primaryDNS(192, 168, 1, 1);       // optional
IPAddress secondaryDNS(192, 168, 1, 1);     // optional

String apiKey;          // Variable to store the API key
String phone_number;    // Variable to store the phone number
String door;            // Variable to store the door status
String location;        // Variable to store the location
String Open;            // Variable to store the open status
String Close;           // Variable to store the close status
String url;             // Variable to store the URL for API calls

WebServer server(80);    // Creates an instance of the WebServer class that listens on port 80

void postData();                      // Function prototype for postData()
void handleSubmit();                  // Function prototype for handleSubmit()
void message_to_whatsapp(String);     // Function prototype for message_to_whatsapp(String)
String urlencode(String);             // Function prototype for urlencode(String)

//writing HTML content in form of String
String htmlContent = R"HTML(
  <!DOCTYPE html>
<html>
<head>
 <link rel="icon" type="image/png" href="https://static.vecteezy.com/system/resources/thumbnails/003/740/289/small_2x/dc-monogram-logo-circle-ribbon-style-design-template-free-vector.jpg">
 
 <title>
   DC 
  </title>
  <script>

    var formSubmitted = false;

function clearInput() {
  event.preventDefault();   // Prevent form submission
      var inputIds = ['location', 'door', 'phone_number','apiKey']; // Array of input IDs
      
      for (var i = 0; i < inputIds.length; i++) {
        document.getElementById(inputIds[i]).value = '';
      }
    }

    function toggleContent() {
  var content = document.querySelector('.hide');
  var button = document.querySelector('button');
  
  if (content.style.display === 'none') {
    content.style.display = 'block';
    button.textContent = 'Read Less';
  } else {
    content.style.display = 'none';
    button.textContent = 'Read More';
  }
}

function showPage(pageId) {

      // Hide all pages
      var pages = document.getElementsByClassName("content");
      for (var i = 0; i < pages.length; i++) {
        pages[i].style.display = "none";
      }

      // Show selected page
      var selectedPage = document.getElementById(pageId);
      selectedPage.style.display = "block";
    }

    window.onbeforeunload = function() {
      if (!formSubmitted && (document.forms["myForm"]["phone_number"].value !== "" || document.forms["myForm"]["apiKey"].value !== "")) {
        return "Are you sure you want to leave this page? Your unsaved data will be lost.";
      }
    };

    function validateForm() {
      var phone_number = document.forms["myForm"]["phone_number"].value;
      var apiKey = document.forms["myForm"]["apiKey"].value;

      if (phone_number === "" || apiKey === "") {
        alert("Please fill in all required fields.");
        return false;
      }
      formSubmitted = true;
    }
  </script>

    <style>

.hide {
  display: none;
}

.a{
  background-color: black;
  color: white;
}


    .blue-text {
      color: #1c1c68;
    }

    .blue-text1 {
      color: rgb(207, 92, 92);
    }

    .abc-text {
       position: absolute;
      color: black;
       top: 250px;
    }

    .bc-text {
      background-color: black;
      color: white;
      width: 200px; 
      height: 50px;
    }

    .centered-text {
      text-align: center; 
    }

    .colored-section {
      background-color:black;
      height: 10px; 
      margin-top: 5px;
      }

    .colored-section1 {
      background-color:black;
      height: 30px; 
      }
  
     body {
     background-image: url('https://wtec.io/wp-content/uploads/2020/10/Loesung_1-scaled.jpg'); 
      background-repeat: no-repeat; 
      background-size: cover;
    }

    @media (max-width: 768px) { 
      body {
        background-size: contain;
        background-position: center;
      }
    }

.container {
  position: relative;
}

.container img {
  position: absolute;
  top: 5px;
  left: 0;
}

.container .text {
  margin-left: 30px;
  margin-top: 10px;
}

 .menu {
      display: flex;
      flex-direction: row;
    }

    .menu-item {
      margin-right: 40px;
      cursor: pointer;
      padding: 5px;
      color: white;
    }

.s{
  font-family: Cambria, sans-serif;
}

.s1{
  font-family: Garamond, sans-serif;
}

 .content {
      display: none;
    }

    .circle {
        position: absolute;
        top: 260px;
        width: 20px;
        height: 20px;
        background-color: red;
        border-radius: 50%;
      }

    </style>
</head>
<body>
    <div class="container">
      <img src="https://static.vecteezy.com/system/resources/thumbnails/003/740/289/small_2x/dc-monogram-logo-circle-ribbon-style-design-template-free-vector.jpg" alt="Error" width="30" height="30">
      <h1 class="text s"><strong>DC</strong></h1>
    </div>

     <div class = "colored-section">
     </div><br>

    <div class="blue-text centered-text s"><strong><h2><u>SMART DOOR STATUS</u></h2><strong></div><hr>

    <div class="menu colored-section1">
    <div class="menu-item" onclick="showPage('home')">Home</div>
    <div class="menu-item" onclick="showPage('page2')">Whatsapp Door Notifier</div>
    <div class="menu-item" onclick="showPage('circle')">Status</div>
    <div class="menu-item" onclick="showPage('help')">Contact Us</div>
    <div class="menu-item" onclick="showPage('AU')">About Us</div>
      </div><hr>
  
    <div id="home" class="content" style="display: block;">
      <div class="s1">
      <div><p>Introducing the Smart Door Status and WhatsApp Messaging Webpage!</p></div>

<div><p>Welcome to our innovative solution that brings together the power of technology and convenience to enhance your door monitoring experience. With our webpage, you can easily check the status of your door - whether it's open or closed - and even receive WhatsApp messages through an ESP32 board. Let's </p></div>

<div><p>Experience the future of door monitoring and messaging integration with Smart Door Status. Enjoy peace of mind, stay connected, and enhance the security of your home or office. Get started today and take control of your door status like never before!</p></div>
<br><br><br><br></div></div>

    
      <div id="AU" class="content"><div class="s1">
<div><p>Have you ever been unsure about whether you left your door open or closed? Our Smart Door Status webpage is here to put your mind at ease. By simply accessing the webpage from your computer or smartphone, you'll be able to instantly view the current status of your door. No more second-guessing or rushing back home to check if everything is secure. We provide you with real-time information at your fingertips.</p></div>

<div><p>But that's not all. We've taken convenience to the next level by integrating WhatsApp messaging with our system. Using an ESP32 board, you can receive important notifications, reminders, or alerts directly on your WhatsApp account. Whether it's a friendly reminder to close the door, a notification about an attempted entry, or simply a message to let you know someone is at the door, our system ensures you stay connected and informed, no matter where you are.</p></div>

<div class="hide"><div><p>Our solution is designed with simplicity and user-friendliness in mind. You don't need any technical expertise to use our webpage and ESP32 board. We've made it as intuitive as possible, allowing you to easily access the door status and configure the WhatsApp messaging settings. We prioritize your convenience and strive to deliver a seamless experience from start to finish.</p></div>

<div><p>With our Smart Door Status and WhatsApp Messaging Webpage, you can enjoy peace of mind, knowing that you're always in control of your door's status and stay connected through the popular messaging platform. Whether you're at work, traveling, or simply in another room, our solution keeps you informed and ensures the security of your home or office.</p></div>

<div><p>Experience the future of door monitoring and messaging integration with our revolutionary system. Stay connected, stay secure, and enjoy the convenience of our Smart Door Status and WhatsApp Messaging Webpage.</p></div></div>
<button class="a" onclick="toggleContent()">Read More</button>
<br></div></div>

    <div id="circle" class="content">
      <div class="s">Status Of Door:-</div><br><br><br><br>
      <div id="circle" class="circle"></div>
    </div>

  <div id="page2" class="content">
    <div class="centered-text s"><h3><u>Welcome To Whatsapp Door Notifier</u></h3></div>
    <div class="s">
     <h4>Fill Up The Following Details:</h4>
    </div>

  <form name="myForm" onsubmit="return validateForm()" action='/submit' method='POST'>
    
  <div class="s">
    <label for="location">Enter The Location:-</label>
    <input type='text' name='location' id="location">
    <button onclick="clearInput()">Cancel</button></div><br>
  
  <div class="s">
    <label for="door">Enter The Door:-</label>
    <input type='text' name='door' id="door">
    <button onclick="clearInput()">Cancel</button></div><br>
   
  <div class="s">
   <label for="Open">Do You Want To Get Messages For Opening The Door?:</label>
   <input type='checkbox' name='Open' value='1' id="Open">
  </div><br>

  <div class="s">
   <label for="Close">Do You Want To Get Messages For Closing The Door?:</label>
   <input type='checkbox' name='Close' value='1' id="Close">
  </div><br>
 
  <div class="s">
    <label for="phone_number">Enter The Phone Number*:-</label>
    <input type='text' name='phone_number' required id="phone_number" >
    <button onclick="clearInput()">Cancel</button>
  </div>

  <div class="s">
   <h4><strong> Save The Number "+34 644 41 87 20" Or "+34 644 66 32 62" And Send Whatsapp Message "I allow callmebot to send me messages" To This Number.</strong></h4>
  </div>

  <div class="s">
    <label for="apiKey">Enter The API Key*:-</label>
    <input type='text' name='apiKey' required id="apiKey" >
    <button onclick="clearInput()">Cancel</button></div><br>

  <div><input type='submit' value='SUBMIT' class="bc-text"></div>

  <div class="s"><h5><strong><em> * : compulsory to fill.</em></strong></h5></div></form></div>

  <div id="help" class="content"> 
  <div class="s"><h3>If You need any help, You can contact us on:- </h3></div>
  <div class="s">Email:- <a href="mailto:dychandekar.2004@gmail.com">dychandekar.2004@gmail.com</a></div>
  <div class="s">Contact/Whatsapp No.:- <a href="tel:+918799579699">+918799579699</a></div>
  </div>
 <div class="centered-text s">Support us:-</div><br>
<div class="centered-text"><a href="https://www.instagram.com/dc__288/"><link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<i class="fa fa-instagram" style="font-size:48px;color:red"></i>
<br>
</a>
  <a href="https://www.facebook.com/profile.php?id=100069341554937"><link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<i class="fa fa-facebook-official" style="font-size:48px;color:blue"></i>
<br>
</a></div>
</html>
)HTML";

void handleRoot()
{
  String modifiedContent = htmlContent;                  // Create a copy of the HTML content

  if (digitalRead(dotSensor2Pin) == HIGH)                // Check the state of dotSensor2Pin
  {
    modifiedContent.replace("red", "green");             // Replace "red" with "green" in the copied HTML content
  }
  server.send(200, "text/html", modifiedContent);        // Send the modified HTML content as the response
}

void setup()
{
  pinMode(dotSensor1Pin, INPUT);       // Set dotSensor1Pin as an input pin
  pinMode(dotSensor2Pin, INPUT);       // Set dotSensor2Pin as an input pin

  Serial.begin(9600);               // Initialize the serial communication at a baud rate of 9600

  Wire.begin();                      // Initialize the I2C communication
  WiFi.begin(ssid, password);        // Connect to the Wi-Fi network using the provided SSID and password

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");       // Display "Connecting to WiFi..." while waiting for the connection
  }
  Serial.println("Connected to WiFi");             // Print "Connected to WiFi" once the connection is established

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);                  // Configure the system time using NTP server

  WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS);          // Configure the static IP address, gateway, subnet, and DNS servers

  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");          // Print an error message if failed to configure the static IP
  }

  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());              // Print the local IP address assigned to the device

  server.on("/", handleRoot);                  // Register the handleRoot() function to handle requests to the root path ("/")
  server.on("/submit", handleSubmit);          // Register the handleSubmit() function to handle requests to the "/submit" path

  server.begin();                              // Start the web server
  Serial.println("Web server started");        // Print "Web server started" once the server is up and running
}

void loop()
{

  int dotSensor1State = digitalRead(dotSensor1Pin);            // Read the state of dotSensor1Pin
  int dotSensor2State = digitalRead(dotSensor2Pin);            // Read the state of dotSensor2Pin

  if (dotSensor1State == HIGH)
  {
    Serial.print("Dot sensor 1 is on ");                   

    if (Open == "1")                                     // Assuming "Open" is a variable of type string
    {
      message_to_whatsapp("Door is open.🔴");           // Call a function to send a message to WhatsApp
    }
  }
  else
  {
    Serial.print("Dot sensor 1 is off ");
  }
  if (dotSensor2State == HIGH)
  {
    Serial.println("Dot sensor 2 is on");

    if (Close == "1")                                     // Assuming "Close" is a variable of type string
    {
      message_to_whatsapp("Door is closed.🟢");          // Call a function to send a message to WhatsApp
  }
  }
  else
  {
    Serial.println("Dot sensor 2 is off");
  }
  
  if (time(&now))
  {
    localtime_r(&now, &timeinfo);                                                      // Get the local time
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);          // Format the time as a string
    Serial.println(timeString);                                                        // Print the formatted time to the serial monitor
  }
  delay(1000);                    // Delay for 1 second
  server.handleClient();          // Handle incoming client requests
  delay(1000);                    // Delay for 1 second
}
void message_to_whatsapp(String message)
{
  url = "https://api.callmebot.com/whatsapp.php?phone=" + phone_number + "&apikey=" + apiKey + "&text=" + urlencode(location + " " + door + " " + message + " " + timeString);

  postData();          // Call the postData() function to send the message
}

void postData()
{
  int httpCode;
  HTTPClient http;
  http.begin(url);                       // Specify the URL to send an HTTP POST request to
  httpCode = http.POST(url);             // Send the HTTP POST request and get the response code
  if (httpCode == 200)
  {
    Serial.println("Sent ok.");          // Print "Sent ok." if the request was successful
  }
  else
  {
    Serial.println("Error.");            // Print "Error." if there was an error in the request
  }
  http.end();                            // Close the HTTP connection
}

String urlencode(String str)
{
  String encodedString = "";                    // Initialize an empty string to store the encoded characters
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++)
  {
    c = str.charAt(i);
    if (c == ' ')
    {
      encodedString += '+';           // Replace spaces with the '+' character
    }
    else if (isalnum(c))              // If the character is alphanumeric
    {
      encodedString += c;             // Append it as is to the encoded string
    }
    else                              // If the character is a special character
    {
      code1 = (c & 0xf) + '0';         // Get the lower 4 bits and convert it to the corresponding character
      if ((c & 0xf) > 9)
      {
        code1 = (c & 0xf) - 10 + 'A';  // If it is greater than 9, convert it to the corresponding hexadecimal character (A-F)
      }
      c = (c >> 4) & 0xf;            // Get the upper 4 bits and shift them to the lower 4 bits
      code0 = c + '0';               // Convert it to the corresponding character
      if (c > 9)
      {
        code0 = c - 10 + 'A';        // If it is greater than 9, convert it to the corresponding hexadecimal character (A-F)
      }
      code2 = '\0';
      encodedString += '%';        // Append the '%' character
      encodedString += code0;      // Append the first hexadecimal character
      encodedString += code1;      // Append the second hexadecimal character
    }
    yield();                  // Yield to other tasks (if any) to prevent blocking
  }
  return encodedString;       // Return the final encoded string
}

void handleSubmit()
{
  // Retrieving values from HTTP request parameters
  phone_number = server.arg("phone_number");
  apiKey = server.arg("apiKey");
  location = server.arg("location");
  door = server.arg("door");
  Open = server.arg("Open");
  Close = server.arg("Close");

  // HTML content to be sent as a response
  String htmlContent2 = R"HTML(
  <!DOCTYPE html>
<html>
<head>
 <link rel="icon" type="image/png" href="https://static.vecteezy.com/system/resources/thumbnails/003/740/289/small_2x/dc-monogram-logo-circle-ribbon-style-design-template-free-vector.jpg">
 <title>
   DC 
  </title>
    <style>

     body {
     background-image: url('https://wtec.io/wp-content/uploads/2020/10/Loesung_1-scaled.jpg'); 
      background-repeat: no-repeat;
      background-size: cover;
    }

    @media (max-width: 768px) { 
      body {
        background-size: contain; 
        background-position: center; 
      }
    }

    .container {
  position: relative;
  }

.container img {
  position: absolute;
  top: 5px;
  left: 0;
}

 .centered-text {
      text-align: center; 
    }

.container .text {
  margin-left: 30px;
  margin-top: 10px;
}

.s{
  font-family: Cambria, sans-serif;
}

  .circle {
      width: 20px;
      height: 20px;
      background-color: red;
      border-radius: 50%;
  }

 .colored-section {
      background-color:black;
      height: 30px; 
      }

    </style>
</head>
<body>
 <div class="container">
      <img src="https://static.vecteezy.com/system/resources/thumbnails/003/740/289/small_2x/dc-monogram-logo-circle-ribbon-style-design-template-free-vector.jpg" alt="Error" width="30" height="30">
      <h1 class="text s"><strong>DC</strong></h1> 
    </div><br>

     <div class = "colored-section">
     </div><br>

   <h4 class="s">
    <em><strong>Submitted Successfully!</strong></em>
   </h4>

   <div class="centered-text s">Support us:-</div><br>
<div class="centered-text"><a href="https://www.instagram.com/dc__288/"><link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<i class="fa fa-instagram" style="font-size:48px;color:red"></i>
<br>
</a>
  <a href="https://www.facebook.com/profile.php?id=100069341554937"><link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<i class="fa fa-facebook-official" style="font-size:48px;color:blue"></i>
<br>
</a></div>
</body>
</html>
)HTML";
  server.send(200, "text/html", htmlContent2);    // Send the HTML content as the response
}