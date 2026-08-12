from .base import BaseRenderer

class LinkerRenderer(BaseRenderer):
    template_name="linker.ld.j2"

    def render_linker(self, memory, output):
        self.render_to_file(output/f"linker.ld", memory=memory)
