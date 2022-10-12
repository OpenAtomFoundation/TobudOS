import uos as os

# test mkdir()
print('{:-^40}'.format('test mkdir'))
print(os.listdir())
os.mkdir('test_dir')
print(os.listdir())

# test file open() and write()
print('{:-^40}'.format('test write'))
f = open('test_dir/test_file', 'w')
f.write("Hello TencentOS-tiny\n")
print("Hello MicroPython", file=f)
f.close()
print(os.listdir('test_dir'))

# test file open() and read()
print('{:-^40}'.format('test read'))
f = open('test_dir/test_file', 'r')
print(f.readlines())
f.close()

# test 'with' statement and iterator
print('{:-^40}'.format('test with statement'))
with open('test_dir/test_file', 'r') as file:
    for line in file:
        print(line)

# test rename()
print('{:-^40}'.format('test rename'))
os.rename('test_dir', 'test_dir2')
print(os.listdir())
print(os.listdir('test_dir2'))

# test unlink()
print('{:-^40}'.format('test unlink'))
os.unlink('test_dir2/test_file')
print(os.listdir('test_dir2'))
os.unlink('test_dir2')
print(os.listdir())
