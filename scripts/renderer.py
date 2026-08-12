from pathlib import Path
from jinja2 import Environment, FileSystemLoader

def create_environment(filters=None):
    env=Environment(
        loader=FileSystemLoader(Path(__file__).parent.parent/"templates"),
        trim_blocks=True,
        lstrip_blocks=True,
    )
    if filters:
        env.filters.update(filters)
    return env
