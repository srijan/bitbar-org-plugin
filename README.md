## Org Mode TODO Plugin

[Bitbar](https://github.com/matryer/bitbar) plugin to display the number of open Org mode TODO items. This entire program could easily be replaced by a simple shell script, but where's the fun in that...

### Installation

No external dependencies are required. Configuration is handled by the
macro constants `CONFIG_DIR` and `MAX_PRINT`. The former is used in
the glob pattern `CONFIG_DIR/*.org`. The latter controls the number of
entries in the bitbar dropdown menu.

```sh
$ cc -O2 -Wall -Wextra plugin.c -o plugin
```

To enable, link to bitbar's plugin directory with an added useless suffix since bitbar is unable to match the refresh interval portion of a plugin filename if it isn't formatted exactly as three period separated fields.

```sh
$ ln -s $PWD/plugin <plugin_dir>/plugin.2h.x
```
