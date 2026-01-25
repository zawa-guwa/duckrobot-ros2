#!/bin/bash

# エラーが起きたら即終了
set -e

# ROS2 ワークスペースのセットアップ
echo "Sourcing ROS2 workspace..."
source ~/ros2_ws/install/setup.bash

# rosbridge をバックグラウンドで起動
echo "Starting rosbridge..."
( ros2 launch rosbridge_server rosbridge_websocket_launch.xml ) &
ROSBRIDGE_PID=$!

echo "Starting usb_cam launch..."
ros2 launch usb_cam usb_cam_node_components.launch.py &
USBCAM_PID=$!

echo "Starting motor_control launch..."
ros2 launch motor_control motor_control_node_components.launch.py &
MOTOR_PID=$!

# npm dev サーバー起動（フロントエンド）
echo "Starting npm dev server..."
cd ~/webapp
npm run dev &
NPM_PID=$!

# 終了処理
echo "All processes started."
echo "Press Ctrl+C to stop everything."

# Ctrl+C を受け取ったら全部終了
trap "echo 'Stopping...'; kill $ROSBRIDGE_PID $USBCAM_PID $MOTOR_PID $WEB_PID; exit" SIGINT

# 無限待機（プロセスが動いている限りスクリプトは終了しない）
wait