import random
import time

from constants import *
from network import *


def main():
    side = random.randint(0, 1)
    if side == 0:
        side = "buy"
    else:
        side = "sell"
    while True:
        order = {
            "type": "limit",
            "security_id": DEFAULT_SECURITY_ID,
            "client_id": "UNIFROM",
            "side": side,
            "quantity": random.randint(1, 10),
            "price": random.uniform(1, MAX_PRICE),
        }

        send_to_gateway(order)

        time.sleep(1)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
