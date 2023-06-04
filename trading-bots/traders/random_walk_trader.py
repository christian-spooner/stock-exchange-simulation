import random
import time

from constants import *
from network import *


def main():
    current_price = MEAN_PRICE

    while True:
        side = random.randint(0, 1)
        if side == 0:
            side = "buy"
        else:
            side = "sell"
        order = {
            "type": "limit",
            "security_id": DEFAULT_SECURITY_ID,
            "client_id": "RANDOM_WALK",
            "side": side,
            "quantity": random.randint(1, 10),
            "price": current_price,
        }

        send_to_gateway(order)

        price_change = random.uniform(-1, 1)
        current_price += price_change
        time.sleep(5)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
