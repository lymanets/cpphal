from pathlib import Path

import click

from .generator import generate


@click.command()
@click.argument("configs", type=click.Path(exists=True))
@click.argument("manifest", type=click.Path(exists=True))
@click.argument("svd_dir", type=click.Path(exists=True))
@click.option("-o", "--output", type=click.Path())
def main(configs, manifest, svd_dir, output):
    generate(
        Path(configs),
        Path(manifest),
        Path(svd_dir),
        Path(output),
        "mcu"
    )


if __name__ == "__main__":
    main()
