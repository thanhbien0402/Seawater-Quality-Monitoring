import { Button } from '../ui/button'
import {
    Dialog,
    DialogClose,
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogTrigger
} from '../ui/dialog'
import { ClipboardList } from 'lucide-react'
import DemoPage from '@/app/tasks/page'

export function TaskListModal() {

    return (
        <Dialog>
            <DialogTrigger asChild>
                <Button variant="ghost" className="gap-1">
                    <ClipboardList className="h-4 w-4" />
                    <p>Tasks</p>
                </Button>
            </DialogTrigger>
            <DialogContent className="sm:max-w-[1080px]">
                <DialogHeader>
                    <DialogTitle>Task List</DialogTitle>
                    <DialogDescription>
                        View your task list below.
                    </DialogDescription>
                </DialogHeader>
                <DemoPage />
                <DialogFooter>
                    <DialogClose asChild>
                        <Button type="button" variant="default">
                            Close
                        </Button>
                    </DialogClose>
                </DialogFooter>
            </DialogContent>
        </Dialog>
    )
}
