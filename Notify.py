# -*- coding:utf-8 -*-
import requests
import sys

def main():
    send_notify('人数オーバーしました．現在の人数: ' + sys.argv[1])

def send_notify(notification_message):
    
    line_notify_token = 'ywdBpwRENoeGwzAAIEdToLGUXo2P2PlbtlKuWRmpJP0'
    line_notify_api = 'https://notify-api.line.me/api/notify'
    #For python3.6 and after
    #headers = {'Authorization': f'Bearer {line_notify_token}'}
    #data = {'message': f'message: {notification_message}'}
    #For others
    headers = {'Authorization': 'Bearer '+ line_notify_token}
    data = {'message': 'message: ' + notification_message}
    requests.post(line_notify_api, headers = headers, data = data)

if __name__ == "__main__":
    main()