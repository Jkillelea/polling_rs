use std::mem;
use std::fs::File;
use std::os::raw::*;
use std::os::unix::io::AsRawFd;
use std::thread;
use std::time::Duration;

mod imports;

static mut COUNTER: i32 = 0;

#[no_mangle]
extern fn sigpoll_callback(signal: c_int) {
    unsafe {
        println!("{} callback {}", COUNTER, signal);
        COUNTER = COUNTER + 1;
    }
}

fn main() {
    let f = File::open("/dev/ttyS0").unwrap();
    let fd = f.as_raw_fd();

    unsafe {
        use imports::*;
        fcntl(fd as c_int, F_SETOWN as i32, getpid());
        fcntl(fd as c_int, F_SETFL as i32, O_NONBLOCK | O_ASYNC);

        let sa = sigaction {
            __sigaction_handler: sigaction__bindgen_ty_1 {
                sa_handler: Some(sigpoll_callback),
            },

            sa_flags: 0,

            sa_mask: __sigset_t {
                __val: [0; mem::size_of::<__sigset_t>() / mem::size_of::<c_ulong>()]
            },

            sa_restorer: None,
        };

        let mut oldsa = mem::zeroed();
        sigaction(SIGIO as i32, &sa, &mut oldsa);
    }

    loop {
        thread::sleep(Duration::from_secs(1));
    }
}
