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

References: https://github.com/LedgerHQ/ledger-app-builder
