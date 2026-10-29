import asyncio
import struct
import time
import psutil


async def test_client(i):
    reader, writer = await asyncio.open_connection(
        "127.0.0.1", 8080
    )

    try:
        for n in range(1000):
            message = f"Client {i}, message {n}".encode()

            header = struct.pack("!I", len(message))

            writer.write(header + message)
            await writer.drain()

            header = await reader.readexactly(4)
            size = struct.unpack("!I", header)[0]

            response = await reader.readexactly(size)

            assert response == message

    finally:
        writer.close()
        await writer.wait_closed()


async def monitor(server, measurements):
    while True:
        memory = server.memory_info().rss / (1024 * 1024)
        threads = server.num_threads()

        measurements.append((memory, threads))

        await asyncio.sleep(0.05)


async def main():
    pid = int(input("Введи PID сервера: "))
    server = psutil.Process(pid)

    for count in [1, 10, 20, 50, 100]:
        measurements = []

        monitor_task = asyncio.create_task(
            monitor(server, measurements)
        )

        cpu_before = server.cpu_times()
        start = time.perf_counter()

        try:
            await asyncio.wait_for(
                asyncio.gather(
                    *(test_client(i) for i in range(count))
                ),
                timeout=60
            )
        finally:
            monitor_task.cancel()
            await asyncio.gather(
                monitor_task, return_exceptions=True
            )

        elapsed = time.perf_counter() - start
        cpu_after = server.cpu_times()

        cpu_seconds = (
            cpu_after.user + cpu_after.system
            - cpu_before.user - cpu_before.system
        )

        cpu = cpu_seconds / elapsed * 100

        memory = max(m[0] for m in measurements)
        threads = max(m[1] for m in measurements)

        messages = count * 1000
        speed = messages / elapsed

        print(
            f"{count} клиентов | "
            f"{speed:.0f} msg/s | "
            f"CPU: {cpu:.1f}% | "
            f"RAM: {memory:.1f} MB | "
            f"Threads: {threads}"
        )


asyncio.run(main())
