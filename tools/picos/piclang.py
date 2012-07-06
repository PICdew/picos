
class PCB(dict):
    """
    Abstraction Layer for the PICLANG Program Control Block (PCB)
    """
    def __init__(self,datablock):
        import picos.core as pcore
        word_size = pcore.word_size
        if len(datablock) != pcore.pcb_size:
            raise Exception("Not a valid PCB block. Expected size %d by actual size is %d" % (pcore.pcb_size, len(datablock)))
        index = 0
        self['page_size'] = datablock[index:index+word_size]
        index += word_size
        self['bitmap'] = datablock[index:index+word_size]
        index += word_size
        self['num_pages'] = datablock[index:index+word_size]
        index += word_size
        self['pc'] = datablock[index:index+word_size]
        index += word_size
        self['status'] = datablock[index:index+word_size]
        index += word_size
        self['start_address'] = datablock[index:index+word_size]
        index += word_size
        self['string_address'] = datablock[index:index+word_size]
        index += word_size
        self['stack'] = datablock[index:index+word_size*pcore.stack_size]
        index += word_size*pcore.stack_size
        self['stack_head'] = datablock[index:index+word_size]
        index += word_size
        self['call_stack'] = datablock[index:index+word_size*pcore.call_stack_size]
        index += word_size*pcore.call_stack_size
        self['call_stack_head'] = datablock[index:index+word_size]
        index += word_size
        
def test():
    from picos.core import pcb_size
    return PCB("b"*pcb_size)
