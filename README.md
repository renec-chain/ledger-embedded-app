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
docker run --rm -ti -p 5000:5000 -v "$(pwd -P):/app" --user $(id -u):$(id -g) -v "/tmp/.X11-unix:/tmp/.X11-unix" -e DISPLAY="host.docker.internal:0" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
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
   
   - Run snapshot tests with Ragger framework and Speculos simulator:
      
      + Open another terminal, use this command:
      ```bash
      sudo docker run --rm -ti -v "$(pwd -P):/app" -v "/tmp/.X11-unix:/tmp/.X11-unix" -e DISPLAY="host.docker.internal:0" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
      ```
      + Run the snapshot tests inside the above docker container:
      ```bash
      bash$ pytest tests/python --tb=short -v --device nanos --display
      ```
      + If the above command returns error, install `base58` dependency and rerun the above command:
      ```bash
      bash$ pip install base58
      ```
      + If providing more useful snapshot tests, please use the following command to generate golden snapshot tests:
      ```bash
      bash$ pytest --device nanos --display --golden_run tests/python/test_sample.py -v -o log_cli=true -o log_cli_level=DEBUG
      ```


5. Side-loading development application on Ledger Nano S or Nano S Plus physical devices:
   - First, initialize on the host machine Python virtual environment and activate it:
   ```bash
   python -m venv venv
   source venv/bin/activate
   ```
   - Install ledgerblue dependency:
   ```bash
   pip install ledgerblue
   ```
   - Load the application into physical device:
   ```bash
   python3 -m ledgerblue.loadApp --curve ed25519 --appFlags 0x800  --path "44'/501'" --tlv --targetId 0x33100004 --targetVersion="" --apiLevel 1 --delete --fileName bin/app.hex --appName "RENEC" --appVersion "0.1.0" --dataSize $((0x`cat debug/app.map |grep _envram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'` - 0x`cat debug/app.map |grep _nvram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'`)) `ICONHEX=\`python3 /opt/nanosplus-secure-sdk/icon3.py --hexbitmaponly icons/nanox_app_renec.gif  2>/dev/null\` ; [ ! -z "$ICONHEX" ] && echo "--icon $ICONHEX"`
   ```

References: https://github.com/LedgerHQ/ledger-app-builder
