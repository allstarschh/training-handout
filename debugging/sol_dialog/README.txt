Here are the issues present in lab_dialog, all fixed here:

- Window did not initialize m_ui
- the DateInputDialog was deleted on close, thus you could not put it
  on the stack nor were you allowed to access it after exec().
    (fixed by removing WA_DeleteOnClose in the DateInputDialog constructor)
- the m_ui members were leaked, both in the window and in the dialog
- once all of that worked, you could still run into a signed integer
  overflow when selecting a date far back in the past or far ahead
  in the future. This can be detected by UBSAN and fixed via upcasting.
