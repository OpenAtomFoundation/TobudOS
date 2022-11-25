import _thread

lock = _thread.allocate_lock()
n_thread = 4
n_finished = 0

def thread_entry(no):
    print(no)
    with lock:
        global n_finished
        n_finished += 1

if __name__ == '__main__':
    for i in range(n_thread):
        _thread.start_new_thread(thread_entry, (i,))

    while n_finished < n_thread:
        pass
    print("done")
