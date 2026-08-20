from .doc import DocRenderer

class DocsRenderer():

    def render_doc(self, env, events, peripheral_options, doc_dir):
        DocRenderer(env, "uart", "USART*").render_doc(events, peripheral_options, doc_dir)