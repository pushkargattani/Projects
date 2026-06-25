import numpy as np
import math
import random
import heapq
from dataclasses import dataclass, field


# --------------- TIME-VARYING LAMBDA ---------------

def get_lambda(base_lambda: float, hour: int) -> float:
    morning_peak = math.exp(-0.5 * ((hour - 9) / 2) ** 2)
    evening_peak = math.exp(-0.5 * ((hour - 18) / 2) ** 2)
    peak = max(morning_peak, evening_peak)
    # night floor: at 3am peak≈0, multiplier = 0.1 (very quiet)
    # rush hour: peak≈1, multiplier = 5.0 (very busy)
    multiplier = 0.1 + 4.9 * peak
    return base_lambda * multiplier


# --------------- DISTANCE MATRIX ---------------

ZONE_DISTANCE = {
    ("Airport",     "Downtown"):    20,
    ("Airport",     "College"):     25,
    ("Airport",     "Residential"): 30,
    ("Airport",     "Suburbs"):     35,
    ("Downtown",    "College"):     10,
    ("Downtown",    "Residential"): 15,
    ("Downtown",    "Suburbs"):     25,
    ("College",     "Residential"): 10,
    ("College",     "Suburbs"):     20,
    ("Residential", "Suburbs"):     15,
}

def get_distance(zone_a: str, zone_b: str) -> int:
    if zone_a == zone_b:
        return random.randint(2, 5)
    return ZONE_DISTANCE.get((zone_a, zone_b)) or \
           ZONE_DISTANCE.get((zone_b, zone_a), 99)


# --------------- RIDE REQUEST ---------------

@dataclass
class RideRequest:
    zone: str
    destination: str
    patience: int = 0

    def __post_init__(self):
        # rider waits 0-15 mins before cancelling
        self.patience = random.randint(0, 10)

    def tick(self) -> bool:
        # count down patience, return False when rider cancels
        self.patience -= 1
        return self.patience > 0


# --------------- ZONE ---------------

@dataclass
class Zone:
    name: str
    lambda_rate: float
    available_drivers: int = 0
    current_demand: int = 0
    prev_demand: int = 0
    request_queue: list = field(default_factory=list)

    def generate_demand(self, hour: int, zones: list):
        self.prev_demand = self.current_demand

        # tick existing requests, remove cancelled ones
        self.request_queue = [r for r in self.request_queue if r.tick()]

        # generate new requests this tick
        actual_lambda = get_lambda(self.lambda_rate, hour)
        new_requests = int(np.random.poisson(actual_lambda))

        # add new requests with random destinations
        for _ in range(new_requests):
            destination = random.choice(
                [z.name for z in zones if z.name != self.name]
            )
            self.request_queue.append(RideRequest(
                zone=self.name,
                destination=destination
            ))

        self.current_demand = len(self.request_queue)
        return self.current_demand


# --------------- DRIVER ---------------

@dataclass
class Driver:
    driver_id: int
    current_zone: str
    state: str = "idle"
    time_until_free: int = 0
    pickup_remaining: int = 0
    destination_zone: str = ""

    def get_score(self, pickup_zone: str) -> int:
        if self.state == "idle":
            return get_distance(self.current_zone, pickup_zone)
        else:
            return self.time_until_free + \
                   get_distance(self.destination_zone, pickup_zone)

    def assign_ride(self, pickup_zone: str, destination_zone: str):
        if self.state == "idle":
            travel_to_pickup = get_distance(self.current_zone, pickup_zone)
        else:
            travel_to_pickup = get_distance(self.destination_zone, pickup_zone)

        ride_duration = get_distance(pickup_zone, destination_zone)

        self.pickup_remaining = self.time_until_free + travel_to_pickup
        self.time_until_free  = self.pickup_remaining + ride_duration
        self.destination_zone = destination_zone
        self.state = "busy"

    def tick(self):
        if self.state == "busy":
            self.time_until_free -= 1
            if self.pickup_remaining > 0:
                self.pickup_remaining -= 1
            else:
                self.current_zone = self.destination_zone
            if self.time_until_free <= 0:
                self.state = "idle"
                self.pickup_remaining = 0


# --------------- ZONES AND DRIVERS ---------------

zones = [
    Zone(name="Airport",     lambda_rate=0.15),
    Zone(name="Downtown",    lambda_rate=0.25),
    Zone(name="College",     lambda_rate=0.18),
    Zone(name="Residential", lambda_rate=0.10),
    Zone(name="Suburbs",     lambda_rate=0.05),
]


def get_active_drivers(drivers: list, hour: int) -> list:
    if 8 <= hour <= 10 or 17 <= hour <= 20:
        count = 40    # rush hour — all drivers
    elif 21 <= hour <= 23:
        count = 28    # evening
    elif 0 <= hour <= 5:
        count = 12     # night skeleton crew
    else:
        count = 22    # normal hours
    return drivers[:count]


def create_drivers(zones: list) -> list:
    # 25 total drivers, more in busier zones
    zone_driver_counts = {
        "Airport":     8,
        "Downtown":    10,
        "College":     9,
        "Residential": 7,
        "Suburbs":     6,
    }
    drivers = []
    driver_id = 0
    for zone in zones:
        for _ in range(zone_driver_counts[zone.name]):
            drivers.append(Driver(
                driver_id=driver_id,
                current_zone=zone.name
            ))
            driver_id += 1
    return drivers


# --------------- DISPATCH ---------------

MAX_WAIT_SCORE = 40


def dispatch_drivers(zones: list, drivers: list, hour: int):
    heap = []
    zone_lookup = {z.name: z for z in zones}

    for zone in zones:
        zone.available_drivers = sum(
            1 for d in drivers
            if d.current_zone == zone.name and d.state == "idle"
        )
        urgency = zone.current_demand - zone.available_drivers
        heapq.heappush(heap, (-urgency, zone.name))

    # only idle drivers
    active_drivers = [
        d for d in get_active_drivers(drivers, hour)
        if d.state == "idle"
    ]

    while heap:
        neg_urgency, zone_name = heapq.heappop(heap)
        zone = zone_lookup[zone_name]

        if not zone.request_queue:
            continue

        served = []
        for request in zone.request_queue:
            if not active_drivers:
                break

            best_driver = min(
                active_drivers,
                key=lambda d: d.get_score(zone_name)
            )

            if best_driver.get_score(zone_name) > MAX_WAIT_SCORE:
                break

            best_driver.assign_ride(zone_name, request.destination)
            active_drivers.remove(best_driver)
            served.append(request)

        for r in served:
            zone.request_queue.remove(r)

        zone.current_demand = len(zone.request_queue)


# --------------- SIMULATION LOOP ---------------

def run_simulation(zones: list, drivers: list, total_ticks: int = 1440):
    simulation_log = []

    for tick in range(total_ticks):
        hour   = tick // 60
        minute = tick % 60

        # step 1: generate demand and update queues
        for zone in zones:
            zone.generate_demand(hour, zones)

        # step 2: snapshot BEFORE dispatch
        active = get_active_drivers(drivers, hour)
        for zone in zones:
            idle_count = sum(
                1 for d in active
                if d.current_zone == zone.name and d.state == "idle"
            )
            simulation_log.append({
                "tick":              tick,
                "hour":              hour,
                "minute":            minute,
                "zone":              zone.name,
                "demand":            zone.current_demand,
                "prev_demand":       zone.prev_demand,
                "available_drivers": idle_count,
            })

        # step 3: dispatch
        dispatch_drivers(zones, drivers, hour)

        # step 4: tick every driver
        for driver in drivers:
            driver.tick()

    return simulation_log


# --------------- MAIN ---------------

if __name__ == "__main__":
    drivers = create_drivers(zones)

    print("Running 24-hour simulation...\n")
    log = run_simulation(zones, drivers)

    print(f"Total log entries: {len(log)}")
    print(f"Expected: {1440 * len(zones)}\n")

    for check_minute in [0, 1, 2, 3, 4, 5]:
        entries = [e for e in log
                   if e["hour"] == 9 and e["minute"] == check_minute]
        print(f"3:{check_minute:02d}")
        for entry in entries:
            print(f"  {entry['zone']:12} → "
                  f"demand={entry['demand']} "
                  f"drivers={entry['available_drivers']}")
        print()