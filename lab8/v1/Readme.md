`server` has two threads, each performing a different task.

`relauncher` launches `server` and revives it if it was killed.

To kill all processes:

1) kill the `relauncher`
2) kill the `server`
