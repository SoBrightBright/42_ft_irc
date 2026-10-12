*This project has been created as part of the 42 curriculum by solee, soolee, jimkim.*

## Description

**ft_irc** is an IRC (Internet Relay Chat) server written in C++98.
It accepts connections from standard IRC clients, lets users register with a
server password, exchange private messages, and talk in channels managed by
channel operators.

The server runs as a single process with a single `poll()` loop: every socket
is non-blocking, and all reads and writes go through per-client buffers, so
one slow or misbehaving client never blocks the others.
[RFC 1459](https://www.rfc-editor.org/rfc/rfc1459) and
[RFC 2812](https://www.rfc-editor.org/rfc/rfc2812) are used as the reference
for message format, command behavior and numeric replies.
The reference client used during development is **irssi**.

### Features

| Area | Supported |
|------|-----------|
| Registration | `PASS`, `NICK`, `USER` (welcome replies `001`–`004`) |
| Connection | `PING`, `PONG`, `QUIT` |
| Messaging | `PRIVMSG` to a user or a channel |
| Channels | `JOIN` (incl. `JOIN 0`), `PART`, `TOPIC` |
| Operator commands | `KICK`, `INVITE`, `MODE` |
| Channel modes | `i` invite-only, `t` topic restricted to operators, `k` key, `o` operator, `l` user limit |

Out of scope: server-to-server links, user modes, and commands not listed
above (e.g. `NOTICE`, `NAMES`, `WHO`, `LIST`, `CAP`), which are answered with
`421 ERR_UNKNOWNCOMMAND`.

## Instructions

### Build

```sh
make        # builds ./ircserv  (c++ -Wall -Wextra -Werror -std=c++98)
make clean  # removes object files
make fclean # removes object files and the binary
make re     # fclean + all
```

### Run

```sh
./ircserv <port> <password>
# example
./ircserv 6667 secret
```

- `port`: 1–65535
- `password`: non-empty; every client must send it with `PASS` before registering

Stop the server with `Ctrl+C`.

### Connect with irssi

```
/connect 127.0.0.1 6667 secret
/join #test
/msg someone hello
/topic #test new topic
/mode #test +i
/invite someone #test
/kick #test someone reason
```

### Connect with nc

The server accepts lines ending with either `\r\n` or `\n`.

```sh
nc 127.0.0.1 6667
PASS secret
NICK alice
USER alice 0 * :Alice
JOIN #test
PRIVMSG #test :hello
QUIT :bye
```

Partial input is buffered until a full line arrives, so a command can be sent
in pieces (e.g. type `com`, `Ctrl+D`, `man`, `Ctrl+D`, `d` + `Enter` in nc).

## Technical choices

### Architecture

```
main.cpp                 argument check, signal handling, server start
Client.{hpp,cpp}         per-connection state: nick/user, buffers, timers
network/                 Server: poll() loop, accept, recv/send, disconnect
parse/                   Message (line -> prefix/command/params),
                         Parser + CommandRegistry (command name -> handler)
common/commandlist/      one class per command, all implementing ICommand
common/IrcHelpers, Reply numeric replies and shared helpers
channel/                 Channel: members, operators, invites, modes
```

- **Command dispatch**: each command is a class implementing `ICommand`
  (`needLogin()`, `execute()`), registered by name in `CommandRegistry`.
  Unknown commands get `421`, commands used before registration get `451`.
- **Deferred disconnection**: clients are never closed in the middle of the
  poll loop. They are marked and cleaned up after the loop iteration. When the
  server closes a link on purpose (`QUIT`, wrong password, ping timeout) it
  first sends `ERROR :Closing Link: ...` and closes after the buffer is flushed
  (or after 5 seconds at most).

### Protocol details

- **Message length** (RFC 2812 §2.3: 512 bytes including CR-LF):
  incoming lines longer than 510 bytes are truncated and still processed;
  outgoing lines longer than 510 bytes are truncated before `\r\n` is added.
  Truncation never splits a UTF-8 character (e.g. Hangul) in the middle.
- **Case mapping** (RFC 2812 §2.2): nicknames and channel names are compared
  case-insensitively, with `{}|^` treated as the lower-case forms of `[]\~`.
- **Nicknames**: 1–9 characters, letter or special character first
  (RFC 2812 §2.3.1).
- **Usernames**: a username containing `@` is rejected with `461`.
- **Channel names**: start with `#` or `&`, at most 50 characters, no space,
  comma or `^G`. The first member of a channel becomes its operator.
- **Registration**: a wrong or missing password is answered with
  `464 ERR_PASSWDMISMATCH` and the connection is closed.
- **Liveness**: unregistered clients are dropped after 60 s. A registered
  client that has been silent for 150 s receives `PING :ircserv`; after 300 s
  without any data it is disconnected and channel members see
  `QUIT :Ping timeout`.
- **Flood protection**: more than 8192 bytes without a newline closes the
  connection; a pending output buffer larger than 1 MB (`SendQ exceeded`)
  closes the connection.
- **`RPL_INVITING` (341)** follows the RFC parameter order
  `<channel> <nick>`.

## Resources

- [RFC 1459 — Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459)
- [RFC 2812 — Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/rfc/rfc2812)
- [irssi documentation](https://irssi.org/documentation/)
- `man 2 poll`, `man 2 socket`, `man 2 recv`, `man 2 send`, `man 2 fcntl`

### Use of AI

Claude (Anthropic) was used as an assistant for:

- **Code review**: reviewing the codebase against RFC 1459 / RFC 2812 and
  pointing out deviations (case mapping, message length handling, KICK error
  order, username validation, MODE parameter handling).
- **RFC lookup**: locating and quoting the relevant RFC sections to justify
  each behavior.
- **Documentation**: drafting this README.

All suggested changes were reviewed, adapted and tested by the team before
being merged.

