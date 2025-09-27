class SHVInstance:
    def __init__(self, filename):
        self.used = False
        self.ip = '127.0.0.1'
        self.port = '3755'
        self.user = 'admin'
        self.passw = 'admin!123'
        self.devid = filename
        self.mount = 'test'

        self.tuned = False
        self.updates = False

        self.tree = 'GAVL'
