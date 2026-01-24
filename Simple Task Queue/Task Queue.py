import threading
import queue
import time
import random

class TaskQueue:
    def __init__(self, num_workers=3):
        self.task_queue = queue.Queue()
        self.workers = []
        self.running = True

        for i in range(num_workers):
            worker = threading.Thread(target=self.worker, daemon=True)
            worker.start()
            self.workers.append(worker)

    def submit(self, task, retries=3):
        self.task_queue.put((task, retries))

    def worker(self):
        while self.running:
            try:
                task, retries = self.task_queue.get(timeout=1)
                try:
                    task()
                except Exception as e:
                    if retries > 0:
                        print("Retrying task...")
                        self.submit(task, retries - 1)
                finally:
                    self.task_queue.task_done()
            except queue.Empty:
                continue

    def shutdown(self):
        self.running = False
        for w in self.workers:
            w.join(timeout=1)

# Example usage
def sample_task():
    if random.random() < 0.3:
        raise Exception("Random failure")
    print("Task executed successfully")
    time.sleep(1)

if __name__ == "__main__":
    tq = TaskQueue(num_workers=2)
    for _ in range(5):
        tq.submit(sample_task)

    tq.task_queue.join()
    tq.shutdown()