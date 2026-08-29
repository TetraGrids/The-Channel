#!/usr/bin/env python3

# This script tests that chan launches keyd automatically when keyd is not
# running yet.

import subprocess


def run_chan_wallet_command(command: str, no_auto_keyd: bool):
    """Run the given chan command and return subprocess.CompletedProcess."""
    args = ['./programs/chan/chan']

    if no_auto_keyd:
        args.append('--no-auto-keyd')

    args += 'wallet', command

    return subprocess.run(args,
                          check=False,
                          stdout=subprocess.DEVNULL,
                          stderr=subprocess.PIPE)


def stop_keyd():
    """Stop the default keyd instance."""
    run_chan_wallet_command('stop', no_auto_keyd=True)


def check_chan_stderr(stderr: bytes, expected_match: bytes):
    if expected_match not in stderr:
        raise RuntimeError("'{}' not found in {}'".format(
            expected_match.decode(), stderr.decode()))


def keyd_auto_launch_test():
    """Test that keos auto-launching works but can be optionally inhibited."""
    stop_keyd()

    # Make sure that when '--no-auto-keyd' is given, keyd is not started by
    # chan.
    completed_process = run_chan_wallet_command('list', no_auto_keyd=True)
    assert completed_process.returncode != 0
    check_chan_stderr(completed_process.stderr, b'Failed http request to keyd')

    # Verify that keyd auto-launching works.
    completed_process = run_chan_wallet_command('list', no_auto_keyd=False)
    if completed_process.returncode != 0:
        raise RuntimeError("Expected that keyd would be started, "
                           "but got an error instead: {}".format(
                               completed_process.stderr.decode()))
    check_chan_stderr(completed_process.stderr, b'launched')


try:
    keyd_auto_launch_test()
finally:
    stop_keyd()
