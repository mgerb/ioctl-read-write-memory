# Read/Write memory with IOCTL

Not actually a Corsair utility, just an arbitrary name to not look suspicious.

### create driver
`sc create <driver name> type= kernel binpath="path to driver"`

**NOTE: if errors occur use `sc.exe` instead of `sc`**

### delete driver
`sc delete <driver name>`

### start driver
`sc start <driver name>`

### stop driver
`sc stop <driver name>`

### enable test signing (requires reboot)
`bcdedit /set testsigning on`
