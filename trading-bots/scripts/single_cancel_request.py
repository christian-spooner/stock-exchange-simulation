from network import *


def main():
    cancel = {
        "type": "cancel",
        "order_id": 1,
        "security_id": DEFAULT_SECURITY_ID,
    }

    send_to_gateway(cancel)


if __name__ == "__main__":
    main()
