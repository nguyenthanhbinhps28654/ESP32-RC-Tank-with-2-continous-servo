#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebServer.h>

const char* ssid = "Your SSID";
const char* password = "Password";

#define SERVO1_PIN 12
#define SERVO2_PIN 13

Servo servo1;
Servo servo2;
WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Cấu hình servo cho continuous servo
  servo1.attach(SERVO1_PIN, 1000, 2000);
  servo2.attach(SERVO2_PIN, 1000, 2000);

  // Đảm bảo servo ban đầu dừng
  stopServos();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/forward", HTTP_GET, handleForward);
  server.on("/backward", HTTP_GET, handleBackward);
  server.on("/crossforward", HTTP_GET, handleCrossForward);
  server.on("/crossbackward", HTTP_GET, handleCrossBackward);
  server.on("/stop", HTTP_GET, handleStop);

  server.begin();
}

void loop() {
  server.handleClient();
}

// Quay cùng hướng theo chiều kim đồng hồ
void rotateClockwise() {
  servo1.write(0);    // Điều chỉnh để servo 1 quay cùng chiều
  servo2.write(180);  // Điều chỉnh để servo 2 quay cùng chiều
}

// Quay cùng hướng ngược chiều kim đồng hồ
void rotateCounterClockwise() {
  servo1.write(180);  // Điều chỉnh để servo 1 quay ngược chiều
  servo2.write(0);    // Điều chỉnh để servo 2 quay ngược chiều
}

// Quay chéo: servo 1 ngược, servo 2 xuôi
void rotateCrossForward() {
  servo1.write(180);  // Servo 1 quay ngược chiều kim đồng hồ
  servo2.write(180);  // Servo 2 quay theo chiều kim đồng hồ
}

// Quay chéo: servo 1 xuôi, servo 2 ngược
void rotateCrossBackward() {
  servo1.write(0);    // Servo 1 quay theo chiều kim đồng hồ
  servo2.write(0);    // Servo 2 quay ngược chiều kim đồng hồ
}

// Dừng servo
void stopServos() {
  servo1.write(90);   
  servo2.write(90);
}

void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>UGV Control Station</title>
    <style>
        body { 
            font-family: Arial; 
            text-align: center; 
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
        .button {
            background-color: #4CAF50;
            border: none;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 10px 2px;
            cursor: pointer;
            border-radius: 5px;
            transition: background-color 0.3s;
        }
        .button:active {
            background-color: #45a049;
        }
    </style>
</head>
<body>
    <h1>UGV Control</h1>
    <div>
        <button class='button' 
        ontouchstart='startForward()' 
        ontouchend='stopServos()'
        onmousedown='startForward()' 
        onmouseup='stopServos()'>
    Forward
</button>
<button class='button' 
        ontouchstart='startBackward()' 
        ontouchend='stopServos()'
        onmousedown='startBackward()' 
        onmouseup='stopServos()'>
    Backward
</button>
<button class='button' 
        ontouchstart='startCrossForward()' 
        ontouchend='stopServos()'
        onmousedown='startCrossForward()' 
        onmouseup='stopServos()'>
    Right
</button>
<button class='button' 
        ontouchstart='startCrossBackward()' 
        ontouchend='stopServos()'
        onmousedown='startCrossBackward()' 
        onmouseup='stopServos()'>
    Left
</button>

    </div>

    <script>
        function startForward() {
            fetch('/forward');
        }
        function startBackward() {
            fetch('/backward');
        }
        function startCrossForward() {
            fetch('/crossforward');
        }
        function startCrossBackward() {
            fetch('/crossbackward');
        }
        function stopServos() {
            fetch('/stop');
        }
    </script>
</body>
</html>
  )";
  server.send(200, "text/html", html);
}

void handleForward() {
  rotateClockwise();
  server.send(200, "text/plain", "Quay xuôi");
}

void handleBackward() {
  rotateCounterClockwise();
  server.send(200, "text/plain", "Quay ngược");
}

void handleCrossForward() {
  rotateCrossForward();
  server.send(200, "text/plain", "Quay chéo 1");
}

void handleCrossBackward() {
  rotateCrossBackward();
  server.send(200, "text/plain", "Quay chéo 2");
}

void handleStop() {
  stopServos();
  server.send(200, "text/plain", "Dừng");
}