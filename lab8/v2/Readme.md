`server` has two threads, each performing a different task. When `server` starts it starts `relauncher`. `server` pings `relauncher` periodically (via a message queue) so that the second knows the first it alive. If the `relauncher` detectes that the `server` died relauncher restarts `server`.

When the server is started by the relauncher the call is `server --no-relaunch` which guarantees that the server does not start another relauncher.

To kill all processes:

1) kill the `relauncher`
2) kill the `server`
