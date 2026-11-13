# chgmac

A small command-line tool to change the MAC address of a network interface on Linux.

The change is **temporary**. After a reboot, the interface goes back to its original MAC address.

This project was made for learning purposes. It is my first low-level project. The code may have bugs, so use it at your own risk.

## Requirements

- Linux
- `gcc` (or any C compiler)
- Root permission

## Build

```bash
make
```

## Usage

```bash
sudo ./chgmac [options] <interface> <new_mac>
```

### Options

| Option | Description |
|--------|-------------|
| `-h`   | Show help |
| `-r`   | Set a random valid MAC address (coming soon) |

### Arguments

| Argument      | Description |
|---------------|-------------|
| `<interface>` | Name of the network interface |
| `<new_mac>`   | New MAC address, format `aa:bb:cc:dd:ee:ff` |

## Valid MAC addresses

- Use 6 pairs of hexadecimal digits, separated by `:` (example: `02:11:22:33:44:55`).
- For a normal network card, the first byte should be **even** (not a multicast address) (example: `02`, `00`, `0a`, `f4`).
- Do not use `00:00:00:00:00:00` or `ff:ff:ff:ff:ff:ff`. Most drivers refuse them with the error `cannot assign requested address`, but some drivers may accept them.

## How it works

1. Open a socket (it is only needed for `ioctl`).
2. Read the interface flags (`SIOCGIFFLAGS`) and bring the interface down (`SIOCSIFFLAGS`).
3. Read the current MAC address (`SIOCGIFHWADDR`).
4. Set the new MAC address (`SIOCSIFHWADDR`).
5. Bring the interface up again (`SIOCSIFFLAGS`).

## Status

I made this tool for learning purposes. Ideas for the future:

- [ ] Random MAC address (`-r`)
- [ ] Restore the original MAC address
