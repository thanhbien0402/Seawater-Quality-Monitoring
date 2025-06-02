import {
    Dialog,
    DialogContent,
    DialogDescription,
    DialogHeader,
    DialogTitle,
} from "@/components/ui/dialog"
import { Dispatch, SetStateAction } from "react"
import { TaskForm } from "../forms/TaskForm"

export const TaskModal = ({ isOpen, isSetOpen, date, setDate }: { isOpen: boolean, isSetOpen: Dispatch<SetStateAction<boolean>>, date: Date | undefined, setDate: Dispatch<SetStateAction<Date | undefined>> }) => {
    return (
        <Dialog open={isOpen} onOpenChange={(open) => {
            if (!open) {
                setDate(undefined)
            }
            isSetOpen(open)
        }}>
            <DialogContent>
                <DialogHeader>
                    <DialogTitle>Add Task</DialogTitle>
                    <DialogDescription>
                        {date && <p>You selected {date.toDateString()}</p>}
                    </DialogDescription>
                </DialogHeader>
                <TaskForm date={date} setDate={setDate} isSetOpen={isSetOpen} />
            </DialogContent>
        </Dialog>
    )
}
