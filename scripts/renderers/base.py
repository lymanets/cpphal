from pathlib import Path
from jinja2 import Environment

class BaseRenderer:
    template_name: str

    def __init__(self, env: Environment):
        self._template = env.get_template(self.template_name)

    def render(self, **context):
        return self._template.render(**context)

    def render_to_file(self, path: Path, **context):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(self.render(**context), encoding="utf-8")
