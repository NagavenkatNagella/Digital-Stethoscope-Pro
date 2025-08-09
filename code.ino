#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

// Wi-Fi credentials
const char* ssid = "Uma";
const char* password = "no password";

// LCD setup (0x3F or 0x27 depending on your module)
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// MAX9814 pin (Analog OUT → A0 on ESP8266)
const int micPin = A0;

// Web server
WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // I2C with custom pins (SDA = D2, SCL = D1)
  Wire.begin(D2, D1);

  // LCD Init
  lcd.init();
  lcd.backlight();

 // Step 1: Welcome Message (centered)
lcd.setCursor(3, 0);  // row 0, col 3
lcd.print("Welcome to");
lcd.setCursor(1, 1);  // row 1, col 1
lcd.print("Digital Steth.");
delay(3000);


  // Step 2: Connecting...
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Step 3: WiFi Connected
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  String ipStr = WiFi.localIP().toString();
  // Clear full line before printing IP
  lcd.print(ipStr);
  if(ipStr.length() < 16){
    for(int i=0; i<16 - ipStr.length(); i++) lcd.print(" "); // clear remainder
  }
  delay(3000);

  // Start web server
  server.begin();
  Serial.println("Server started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int micValue = analogRead(micPin);

  // Update LCD without flicker and clear leftover chars on line 1
  lcd.setCursor(0, 0);
  lcd.print("Frequency Value:    "); // fixed text, with trailing spaces to clear old chars

  lcd.setCursor(0, 1);
  char buf[17];
  sprintf(buf, "%-16d", micValue);  // Left align the number padded with spaces
  lcd.print(buf);

  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client");
    String req = "";
    // Read full HTTP request
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        req += c;
        // End of HTTP headers indicated by empty line
        if (req.endsWith("\r\n\r\n")) break;
      }
    }

  Serial.println("Request: " + req);

if (req.indexOf("GET /data") >= 0) {
  // JSON response for /data endpoint
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();
  client.print("{\"value\":");
  client.print(micValue);
  client.println("}");
} else {
  // Serve main page with attractive hospital theme
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();

  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<title>Digital Stethoscope Pro - Medical Monitor</title>");
  client.println("<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>");
  client.println("<style>");
  client.println("* { margin: 0; padding: 0; box-sizing: border-box; }");
  client.println("body { font-family: 'Arial', sans-serif; background: linear-gradient(135deg, #e8f5e8 0%, #a8e6cf 50%, #56ab91 100%); min-height: 100vh; padding: 15px; }");
  client.println(".container { max-width: 1100px; margin: 0 auto; background: linear-gradient(145deg, #ffffff 0%, #f8fdff 100%); border-radius: 25px; box-shadow: 0 25px 50px rgba(0,0,0,0.15); overflow: hidden; border: 3px solid #e8f5e8; }");
  client.println(".header { background: linear-gradient(135deg, #2c5f41 0%, #4a8c6a 50%, #68b892 100%); padding: 35px 25px; text-align: center; position: relative; }");
  client.println(".header::before { content: ''; position: absolute; top: 0; left: 0; right: 0; bottom: 0; background: url('data:image/svg+xml,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 100 100\"><circle cx=\"20\" cy=\"20\" r=\"2\" fill=\"%23ffffff\" opacity=\"0.1\"/><circle cx=\"80\" cy=\"40\" r=\"1\" fill=\"%23ffffff\" opacity=\"0.1\"/><circle cx=\"40\" cy=\"80\" r=\"1.5\" fill=\"%23ffffff\" opacity=\"0.1\"/></svg>'); }");
  client.println(".header h1 { color: white; font-size: 2.8rem; margin-bottom: 8px; font-weight: 300; text-shadow: 0 2px 4px rgba(0,0,0,0.3); position: relative; z-index: 1; }");
  client.println(".header .subtitle { color: #b8e6d1; font-size: 1.2rem; position: relative; z-index: 1; }");
  client.println(".main-content { padding: 35px 25px; background: linear-gradient(135deg, #fafffe 0%, #f0f9f4 100%); }");
  client.println(".stats-row { display: flex; gap: 20px; margin-bottom: 35px; flex-wrap: wrap; }");
  client.println(".stat-card { flex: 1; min-width: 280px; background: linear-gradient(145deg, #ffffff 0%, #f8fdfa 100%); padding: 25px; border-radius: 20px; box-shadow: 0 10px 30px rgba(44, 95, 65, 0.1); text-align: center; border: 2px solid #e8f5e8; position: relative; overflow: hidden; }");
  client.println(".stat-card::before { content: ''; position: absolute; top: -50%; left: -50%; width: 200%; height: 200%; background: linear-gradient(45deg, transparent, rgba(168, 230, 207, 0.1), transparent); transform: rotate(45deg); transition: all 0.6s; }");
  client.println(".stat-card:hover::before { left: 100%; }");
  client.println(".stat-icon { font-size: 2.5rem; margin-bottom: 15px; }");
  client.println(".stat-value { font-size: 2.8rem; font-weight: bold; color: #2c5f41; margin-bottom: 10px; position: relative; z-index: 2; }");
  client.println(".stat-label { color: #4a8c6a; font-size: 1.1rem; text-transform: uppercase; letter-spacing: 1px; font-weight: 600; position: relative; z-index: 2; }");
  client.println(".chart-section { background: linear-gradient(145deg, #ffffff 0%, #f8fdfa 100%); padding: 30px; border-radius: 20px; box-shadow: 0 15px 35px rgba(44, 95, 65, 0.1); border: 2px solid #e8f5e8; }");
  client.println(".chart-header { display: flex; align-items: center; justify-content: center; margin-bottom: 25px; }");
  client.println(".chart-title { font-size: 1.8rem; color: #2c5f41; font-weight: 600; margin-left: 15px; }");
  client.println(".pulse-dot { width: 18px; height: 18px; border-radius: 50%; background: linear-gradient(135deg, #27ae60, #2ecc71); animation: pulse 2s infinite; box-shadow: 0 0 20px rgba(46, 204, 113, 0.6); }");
  client.println("@keyframes pulse { 0%, 100% { transform: scale(1); opacity: 1; } 50% { transform: scale(1.2); opacity: 0.7; } }");
  client.println(".current-pulse { animation: heartbeat 1.5s infinite; }");
  client.println("@keyframes heartbeat { 0%, 100% { transform: scale(1); } 25% { transform: scale(1.1); } 50% { transform: scale(1); } 75% { transform: scale(1.05); } }");
  client.println(".footer { background: linear-gradient(135deg, #e8f5e8, #d4f1e8); padding: 20px; text-align: center; color: #2c5f41; font-size: 0.9rem; }");
  client.println("@media (max-width: 768px) { .stats-row { flex-direction: column; } .stat-card { min-width: auto; } }");
  client.println("</style>");
  client.println("</head><body>");
  
  client.println("<div class='container'>");
  client.println("<div class='header'>");
  client.println("<h1>Digital Stethoscope Pro</h1>");
  client.println("<p class='subtitle'>Advanced Cardiac Monitoring System | Hospital Grade</p>");
  client.println("</div>");
  
  client.println("<div class='main-content'>");
  client.println("<div class='stats-row'>");
  
  client.println("<div class='stat-card'>");
  client.println("<div class='stat-icon'></div>");
  client.println("<div class='stat-value current-pulse' id='currentValue'>0</div>");
  client.println("<div class='stat-label'>Current Reading</div>");
  client.println("</div>");
  
  client.println("<div class='stat-card'>");
  client.println("<div class='stat-icon'></div>");
  client.println("<div class='stat-value' id='avgValue'>0</div>");
  client.println("<div class='stat-label'>Average Level</div>");
  client.println("</div>");
  
  client.println("<div class='stat-card'>");
  client.println("<div class='stat-icon'></div>");
  client.println("<div class='stat-value' id='maxValue'>0</div>");
  client.println("<div class='stat-label'>Peak Signal</div>");
  client.println("</div>");
  
  client.println("</div>");
  
  client.println("<div class='chart-section'>");
  client.println("<div class='chart-header'>");
  client.println("<div class='pulse-dot'></div>");
  client.println("<h3 class='chart-title'>Real-Time Audio Waveform Analysis</h3>");
  client.println("</div>");
  client.println("<canvas id='chart' width='800' height='350'></canvas>");
  client.println("</div>");
  
  client.println("</div>");
  
  client.println("<div class='footer'>");
  client.println("Medical Device Status: Active | Last Update: <span id='lastUpdate'>--</span>");
  client.println("</div>");
  
  client.println("</div>");
  
  client.println("<script>");
  client.println("const ctx = document.getElementById('chart').getContext('2d');");
  client.println("const gradient = ctx.createLinearGradient(0, 0, 0, 350);");
  client.println("gradient.addColorStop(0, 'rgba(46, 204, 113, 0.8)');");
  client.println("gradient.addColorStop(0.5, 'rgba(46, 204, 113, 0.4)');");
  client.println("gradient.addColorStop(1, 'rgba(46, 204, 113, 0.1)');");
  client.println("const data = {labels: [], datasets: [{label: 'Audio Signal', data: [], borderColor: '#27ae60', backgroundColor: gradient, fill: true, tension: 0.4, borderWidth: 3, pointBackgroundColor: '#2ecc71', pointBorderColor: '#27ae60', pointBorderWidth: 2, pointRadius: 4}]};");
  client.println("const config = {type: 'line', data: data, options: {responsive: true, plugins: {legend: {display: false}}, scales: {y: {min: 0, max: 1024, grid: {color: '#e8f5e8'}, ticks: {color: '#4a8c6a'}}, x: {grid: {color: '#e8f5e8'}, ticks: {color: '#4a8c6a'}}}}};");
  client.println("const chart = new Chart(ctx, config);");
  client.println("let values = [];");
  
  client.println("function fetchData() {");
  client.println("  fetch('/data').then(response => response.json()).then(json => {");
  client.println("    const time = new Date().toLocaleTimeString();");
  client.println("    if (data.labels.length > 20) { data.labels.shift(); data.datasets[0].data.shift(); }");
  client.println("    data.labels.push(time);");
  client.println("    data.datasets[0].data.push(json.value);");
  client.println("    values.push(json.value);");
  client.println("    if (values.length > 20) values.shift();");
  client.println("    chart.update('none');");
  client.println("    document.getElementById('currentValue').textContent = json.value;");
  client.println("    document.getElementById('avgValue').textContent = Math.round(values.reduce((a,b) => a+b, 0) / values.length);");
  client.println("    document.getElementById('maxValue').textContent = Math.max(...values);");
  client.println("    document.getElementById('lastUpdate').textContent = time;");
  client.println("  });");
  client.println("}");
  client.println("setInterval(fetchData, 1000);");
  client.println("fetchData();");
  client.println("</script>");
  
  client.println("</body></html>");
}
delay(1);
client.stop();
Serial.println("Client Disconnected");

  delay(500);
}
}
