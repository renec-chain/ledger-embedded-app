# Ledger RENEC Embedded Application

## Overview

This application adds support for RENEC native token to Ledger Nano S/X/SP hardware wallet.

## Usage

1. Pull latest Ledger App Builder Image:

```bash
docker pull ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest
```

2. Compile app in the docker container:

```bash
docker run --rm -ti -v "$(realpath .):/app" --user $(id -u $USER):$(id -g $USER) ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest
bash$ BOLOS_SDK=$NANOS_SDK make
```

3. Run app using XQuartz in simulation mode:

   - Firstly, whitelist `127.0.0.1` in XQuartz server:

   ```bash
   xhost + 127.0.0.1
   ```

   - Secondly, running Ledger dev tools container:

   ```bash
   docker run --rm -ti -v "$(pwd -P):/app" --user $(id -u):$(id -g) -v "/tmp/.X11-unix:/tmp/.X11-unix" -e DISPLAY="host.docker.internal:0" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
   ```

   - Lastly, running simulated app with Speculos simulator inside the container:

   ```bash
   speculos build/nanos/bin/app.elf --model nanos
   ```

References: https://github.com/LedgerHQ/ledger-app-builder
