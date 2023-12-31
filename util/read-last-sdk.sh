#!/usr/bin/env bash
here="$(dirname "$0")"

source "${here}"/active-target.sh

app_root="$(realpath "$here"/..)"
last_target_sdk="$(read_last_target_sdk_mnemonic "$app_root")"
echo $last_target_sdk

if [[ -z "$last_target_sdk" ]]; then
    echo 'cannot resolve target sdk. please run `docker-make <TARGET_SDK>` first' >&2
    exit 1
fi

sdk_mnemonic_to_bolos_variable "$last_target_sdk"