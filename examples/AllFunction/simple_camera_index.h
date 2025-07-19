#ifndef SIMPLE_CAMERA_INDEX_H
#define SIMPLE_CAMERA_INDEX_H

const char simple_index_html[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32-S3 CAM</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            background-color: #f4f4f4;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
        }
        .header {
            text-align: center;
            background-color: #333;
            color: white;
            padding: 15px;
            margin-bottom: 20px;
        }
        .video-container {
            text-align: center;
            background: white;
            padding: 20px;
            border-radius: 5px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        #stream {
            max-width: 100%;
            height: auto;
            border: 2px solid #333;
        }
        .controls {
            margin-top: 20px;
            display: flex;
            justify-content: center;
            gap: 10px;
            flex-wrap: wrap;
        }
        .btn {
            padding: 10px 20px;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            text-decoration: none;
            display: inline-block;
        }
        .btn:hover {
            background-color: #0056b3;
        }
        .info {
            background: white;
            padding: 15px;
            margin-top: 20px;
            border-radius: 5px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>ESP32-S3 Camera</h1>
            <p>SKDJBJG ESP32-S3 WROOM N16R8 CAM with OV2640</p>
        </div>
        
        <div class="video-container">
            <img id="stream" src="/stream" alt="Camera Stream">
        </div>
        
        <div class="controls">
            <a href="/capture" class="btn" target="_blank">Take Photo</a>
            <button class="btn" onclick="toggleStream()">Toggle Stream</button>
            <button class="btn" onclick="refreshStream()">Refresh</button>
        </div>
        
        <div class="info">
            <h3>Camera Information</h3>
            <p><strong>Stream URL:</strong> <code>/stream</code></p>
            <p><strong>Capture URL:</strong> <code>/capture</code></p>
            <p><strong>MQTT Control:</strong> Available for Home Assistant integration</p>
            <p><strong>Board:</strong> ESP32-S3 with OV2640 Camera</p>
        </div>
    </div>

    <script>
        let streamRunning = true;
        
        function toggleStream() {
            const streamImg = document.getElementById('stream');
            if (streamRunning) {
                streamImg.style.display = 'none';
                streamRunning = false;
            } else {
                streamImg.style.display = 'block';
                streamRunning = true;
                refreshStream();
            }
        }
        
        function refreshStream() {
            const streamImg = document.getElementById('stream');
            const currentSrc = streamImg.src;
            streamImg.src = '';
            setTimeout(() => {
                streamImg.src = currentSrc + '?t=' + new Date().getTime();
            }, 100);
        }
        
        // Auto-refresh every 30 seconds to prevent stream timeout
        setInterval(() => {
            if (streamRunning) {
                refreshStream();
            }
        }, 30000);
    </script>
</body>
</html>
)rawliteral";

#endif
