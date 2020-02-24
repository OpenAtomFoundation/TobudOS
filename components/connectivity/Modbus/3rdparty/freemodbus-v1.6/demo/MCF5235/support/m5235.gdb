define setup-and-load
  set $rambar=0x100001
  tbreak start
  tbreak main
end

define debug-printexception
  printf "vector: %d", *(unsigned short *)$sp >> 2 &0x1F
  printf "old pc: 0x%08x", *(unsigned long *)($sp + 4)
  printf "old sr: 0x%02x", *(unsigned short *)($sp + 2)
end

define execute
#  set $sp = *(long *)0x20000
  set $pc = *(long *)0x100004
  tk gdbtk_update
end

define write
  set *0x1000000 = 33
end

define simple
  write
  print /x *0x1000000
end

define wait
  set $delay = 10000
  while $delay > 0
    set $delay = $delay - 1
  end
end

define debug-sramtest
  set $srambase = 0x20000000
  set $sramsize = 0x00010000
  printf "Testing SRAM : 0x%08X - 0x%08X\n", $srambase, ($srambase + $sramsize)
  set $i = $srambase
  while $i < ($srambase + $sramsize)
    set *(unsigned long *)($i) = 0xAA55AA55
    wait
    if 0xAA55AA55 != *(unsigned long *)$i
      printf "  0x%08X = FAIL\n", $i
    else 
      printf "  0x%08X = OK\n", $i
    end
    set $i = $i + 0x1000
  end
end

define debug-sdramtest
  set $sdrambase = 0x00000000
  set $sdramsize = 0x01100000
  printf "Testing SDRAM : 0x%08X - 0x%08X\n", $sdrambase, ($sdrambase + $sdramsize)
  set $i = $sdrambase
  while $i < ($sdrambase + $sdramsize)
    set *(unsigned long *)($i) = 0xAA55AA55
    wait
    if 0xAA55AA55 != *(unsigned long *)$i
      printf "  0x%08X = FAIL\n", $i
    else 
      printf "  0x%08X = OK\n", $i
    end
    set $i = $i + 0x10000
  end
end
