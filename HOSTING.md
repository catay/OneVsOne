# Hosting a 1vs1 League Server

This document describes what is required to build, configure, and host a
BZFlag server for Zongo's 1vs1 BZFlag league using the OneVsOne plug-in.

## Table Of Contents

* [Contacts](#contacts)
* [Prerequisites](#prerequisites)
* [Build The Server](#build-the-server)
* [Server Configuration](#server-configuration)
* [Group Database](#group-database)
* [Supported Styles](#supported-styles)
* [Match Reporting And Logging](#match-reporting-and-logging)
* [Pre-Launch Checklist](#pre-launch-checklist)

## Contacts

For questions or help with setting up a 1vs1 league server, contact one of the
following people:

* [kierra](https://forums.bzflag.org/memberlist.php?mode=viewprofile&u=2229)
* [strayer](https://forums.bzflag.org/memberlist.php?mode=viewprofile&u=227)
* [catay](https://forums.bzflag.org/memberlist.php?mode=viewprofile&u=3438)

You can contact them by private message on the BZFlag forums.

## Prerequisites

Hosting a 1vs1 league server requires working knowledge of how to set up and
operate a BZFlag `bzfs` server.

Before starting, make sure you have:

* A host suitable for running a public BZFlag server.
* The build tools required to compile BZFlag from source.
* A BZFlag server public key for public server listing. Public keys can be
  obtained from [my.bzflag.org/listkeys](https://my.bzflag.org/listkeys/).
* The server IP address or FQDN whitelisted for reporting matches to the 1vs1
  league site.
* Writable paths for match recordings, ban files, report files, and match logs.
* Access to the BZFlag source tree for the latest stable 2.4.x release.

To request reporting access, send a private message to
[strayer](https://forums.bzflag.org/memberlist.php?mode=viewprofile&u=227)
through the BZFlag forums with the server details, including the server IP
address or FQDN.

## Build The Server

Clone the latest stable BZFlag release.

```bash
git clone git@github.com:BZFlag-Dev/bzflag.git
```

Clone this repository into the BZFlag plug-in directory.

```bash
cd bzflag/plugins
git clone git@github.com:catay/OneVsOne.git
cd ..
```

Apply the `fancy` patch from the root of the BZFlag source tree. This is
required when hosting the `fancy` style and is recommended for league hosts that
want one server build capable of running all supported styles.

```bash
patch -p1 < plugins/OneVsOne/misc/fancy-2.4.patch
```

Configure and compile the BZFlag server with the client disabled and the
OneVsOne custom plug-in enabled.

```bash
./autogen.sh
./configure --disable-client --enable-custom-plugins=OneVsOne
make
```

After a successful build, the plug-in should exist at:

```bash
plugins/OneVsOne/.libs/OneVsOne.so
```

## Server Configuration

The example server configurations are in the [examples](examples) directory.
Each style has two files:

* `bzfs.conf`: the `bzfs` server configuration.
* `ovso.ini`: the OneVsOne plug-in configuration.

Before starting a public server, update the placeholders in the selected
`bzfs.conf` file:

* `-publicaddr <INSERT FQDN>:<PORT>` must match the server hostname and port.
* `-publickey <INSERT PUBLIC KEY>` must contain the server public key.
* `-groupdb /path/to/groupdb.1vs1` must point to the group database file.
* `-banfile /path/to/banfile.1vs1` must point to the ban file.
* `-reportfile /path/to/report.1vs1` must point to the report file.
* `-recdir /path/to/recdir` must point to a writable recording directory.

The plug-in is loaded with the matching INI file through `-loadplugin`, for
example:

```text
-loadplugin plugins/OneVsOne/.libs/OneVsOne.so,plugins/OneVsOne/examples/classic/ovso.ini
```

The `ovso.ini` file controls the league style name, maximum lives, league site
communication, and match log path. For official 1vs1 league play,
`max_lives` must be set to `10`.

## Group Database

Use the following group database permissions for a 1vs1 league server.

```text
EVERYONE: -ALL +ACTIONMESSAGE +ADMINMESSAGESEND +DATE +IDLESTATS
EVERYONE: +PRIVATEMESSAGE +SPAWN +TALK +REJOIN +LAGSTATS +SUPERKILL
VERIFIED: -ALL *EVERYONE
1VS1.GOFERS: *EVERYONE +ADMINMESSAGERECEIVE +BAN +BANLIST +KICK +MUTE
1VS1.GOFERS: +PLAYERLIST +UNBAN +UNMUTE +CLIENTQUERY
1VS1.PARIAHS: !ALL
catay.admin: +ALL +OVSO_CFG +HIDEADMIN
PIMPIORG.ADMIN: *catay.admin +OVSO_CFG +HIDEADMIN
```

The `OVSO_CFG` permission allows trusted admins to change OneVsOne plug-in
settings from inside the game, such as the message of the day and maximum life
count.

## Supported Styles

There are currently three configured game styles in this repository.

### Classic

Use the classic example files:

* Server config: [examples/classic/bzfs.conf](examples/classic/bzfs.conf)
* Plug-in config: [examples/classic/ovso.ini](examples/classic/ovso.ini)

The classic server runs on port `5155` in the example configuration and reports
matches with `style = classic`.

### Fancy

Use the fancy example files:

* Server config: [examples/fancy/bzfs.conf](examples/fancy/bzfs.conf)
* Plug-in config: [examples/fancy/ovso.ini](examples/fancy/ovso.ini)

The fancy server runs on port `5156` in the example configuration and reports
matches with `style = fancy`.

The fancy style uses the `-fancy` server option, which is added by
[misc/fancy-2.4.patch](misc/fancy-2.4.patch). Apply the patch before compiling
BZFlag if this style will be hosted.

### Hix

Use the hix example files:

* Server config: [examples/hix/bzfs.conf](examples/hix/bzfs.conf)
* Plug-in config: [examples/hix/ovso.ini](examples/hix/ovso.ini)

The hix server runs on port `5158` in the example configuration and reports
matches with `style = hix`.

The hix map is shipped with the BZFlag source tree and can be found at:

```text
misc/maps/hix.bzw
```

The map is also available from the
[BZFlag 2.4 branch](https://raw.githubusercontent.com/BZFlag-Dev/bzflag/refs/heads/2.4/misc/maps/hix.bzw).

The example hix configuration loads that map with:

```text
-world misc/maps/hix.bzw
```

### Dupi

The `dupi` style is currently not configured in this repository. There is no
example `bzfs.conf` or `ovso.ini` for this style yet, so it should be treated as
TBD until a supported configuration is added.

## Match Reporting And Logging

The plug-in reports official matches to the league site when the selected
`ovso.ini` file contains a valid `httpuri` in the `[communication]` section.

Each configured style also writes match results to the log file configured in
the `[logging]` section. The example log files are:

* Classic: `/var/tmp/classic.log`
* Fancy: `/var/tmp/fancy.log`
* Hix: `/var/tmp/hix.log`

The script [scripts/report_matches.sh](scripts/report_matches.sh) can be
adapted and run from cron to mail logged match results as a backup reporting
path. Update its mail recipients, sender address, subject, and log file paths
before using it.

Official matches are also recorded when `-recdir` is configured and writable.
Do not configure `-recbuf`; the OneVsOne plug-in starts and stops the recording
buffer when an official match starts and ends.

## Pre-Launch Checklist

Before opening the server for league play, verify the following:

* BZFlag was built with `--disable-client --enable-custom-plugins=OneVsOne`.
* `plugins/OneVsOne/.libs/OneVsOne.so` exists.
* The server IP address or FQDN has been whitelisted by
  [strayer](https://forums.bzflag.org/memberlist.php?mode=viewprofile&u=227)
  for match reporting.
* The selected `bzfs.conf` loads the matching `ovso.ini` file.
* `-publicaddr` and `-publickey` are configured.
* `-groupdb`, `-banfile`, `-reportfile`, and `-recdir` point to valid paths.
* The `ovso.ini` file has `max_lives = 10` and the correct `style` value.
* The match log path in `ovso.ini` is writable by the server process.
* The server starts without plug-in load errors.
* In game, `/ovso version` displays the plug-in version.
* In game, `/ovso topzelo` returns the players with the top zelo ranking,
  confirming communication between the server and the 1vs1 league site.
* In game, `/ovso match official` is available for official matches.
