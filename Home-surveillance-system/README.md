# 🏠 Home Surveillance System - Backend
This project is a motion detection-based home surveillance system built using Flask and OpenCV. It captures real-time video, detects motion using frame differencing, and automatically records videos when motion is detected. The system also triggers audio alerts to notify users of detected movement.

# 📌 Features
✅ Live Video Streaming – View real-time camera feed via a web interface.

✅ Motion Detection – Uses OpenCV to detect movement by comparing consecutive frames.

✅ Automated Video Recording – Saves video recordings when motion is detected.

✅ Customizable Sensitivity – Adjust the detection threshold dynamically via an API request.

✅ Audio Alerts – Plays an alarm sound (alert.wav) when motion is detected.

✅ Video Storage – Saves motion-triggered recordings in the recordings/ directory.

<img width="1271" alt="image" src="https://github.com/user-attachments/assets/83026f1d-7c25-431f-b586-c055fc68a1e8" />

# 🛠️ Technologies Used
Python 3 – Main programming language

Flask – Web framework for the backend API

OpenCV – Image processing library for motion detection

Pygame – Used for playing alert sounds

Flask-CORS – Enables cross-origin requests
