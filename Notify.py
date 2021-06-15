import requests

def main():
    send_notify('Test from Mac.')

def send_notify(notification_message):
    
    line_notify_token = 'ywdBpwRENoeGwzAAIEdToLGUXo2P2PlbtlKuWRmpJP0'
    line_notify_api = 'https://notify-api.line.me/api/notify'
    headers = {'Authorization': f'Bearer {line_notify_token}'}
    data = {'message': f'message: {notification_message}'}
    requests.post(line_notify_api, headers = headers, data = data)

if __name__ == "__main__":
    main()