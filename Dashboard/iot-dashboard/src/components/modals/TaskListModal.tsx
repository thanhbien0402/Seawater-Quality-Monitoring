'use client'
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
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs'
import TaskTable from '@/app/tasks/page'
import { TaskForm } from '../forms/TaskForm'
import { useState } from 'react'

export function TaskListModal({ deviceId }: { deviceId: string }) {

    const [isOpen, setIsOpen] = useState<boolean>(false)

    return (
        <Dialog open={isOpen} onOpenChange={(open) => {
            setIsOpen(open)
        }}>
            <DialogTrigger asChild>
                <Button variant="ghost" className="gap-1">
                    <ClipboardList className="h-4 w-4" />
                    <p>Tasks</p>
                </Button>
            </DialogTrigger>
            <DialogContent className="sm:max-w-[1080px]">                <div className="w-full space-y p-3 rounded-md border-4 border-black">
                <Tabs defaultValue="create" className="w-full">
                    <TabsList className="grid w-full grid-cols-2">
                        <TabsTrigger value="create">Create Task</TabsTrigger>
                        <TabsTrigger value="view">View Task</TabsTrigger>
                    </TabsList>
                    <TabsContent value="create">
                        <DialogHeader className='mb-3'>
                            <DialogTitle>Create Task</DialogTitle>
                            <DialogDescription>Create your task by filling the form below</DialogDescription>
                        </DialogHeader>
                        <TaskForm setIsOpen={setIsOpen} deviceId={deviceId} />
                        {/* <SignInForm /> */}
                    </TabsContent>
                    <TabsContent value="view">
                        {/* <RegisterForm /> */}
                        <DialogHeader>
                            <DialogTitle>Task List</DialogTitle>
                            <DialogDescription>
                                View your task list below.
                            </DialogDescription>
                        </DialogHeader>
                        <TaskTable deviceId={deviceId} />
                    </TabsContent>
                </Tabs>
            </div>
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
