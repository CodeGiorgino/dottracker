# dottracker

**dottracker** is a tracking program for UNIX dotfiles and configurations.

## Compilation

```bash
# Clone the repository
git clone git@github.com/andreagiorgino/dottracker.git
cd dottracker

# Build the project
cmake . -B build
cmake --build build -j $((`nproc` - 1))

# Install the binary to local path
ln -fs $(realpath build/dottracker) $HOME/.local/bin
```

> [!IMPORTANT]
> Make sure that "$HOME/.local/bin" exists and that it is part of "$PATH" if
> you want to install the binary

## Usage

```bash
dottracker [-v | --version] [-h | --help] <command> [<args>]
```

### Commands

Command  | Arguments       | Options
:------- | :-------------- | :---------------
`update` | `local`, `repo` | `-s`, `--source`
`diff`   | -               | `-s`, `--source`
