from .base import BaseRenderer

class PeripheralRenderer(BaseRenderer):
    template_name="peripheral.hpp.j2"

    def render_peripheral(self, ctx, namespace, output):
        self.render_to_file(output/f"{ctx.peripheral.name.lower()}.hpp",
                            ctx=ctx,
                            namespace=namespace)
